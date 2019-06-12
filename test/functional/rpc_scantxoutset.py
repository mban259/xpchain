#!/usr/bin/env python3
# Copyright (c) 2018 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the scantxoutset rpc call."""
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal

from decimal import Decimal
import shutil
import os

class ScantxoutsetTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 1
        self.setup_clean_chain = True

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def run_test(self):
        self.log.info("Mining blocks...")
        self.nodes[0].generate(110)

        addr_P2SH_SEGWIT = self.nodes[0].getnewaddress("", "p2sh-segwit")
        pubk1 = self.nodes[0].getaddressinfo(addr_P2SH_SEGWIT)['pubkey']
        addr_LEGACY = self.nodes[0].getnewaddress("", "legacy")
        pubk2 = self.nodes[0].getaddressinfo(addr_LEGACY)['pubkey']
        addr_BECH32 = self.nodes[0].getnewaddress("", "bech32")
        pubk3 = self.nodes[0].getaddressinfo(addr_BECH32)['pubkey']
        self.nodes[0].sendtoaddress(addr_P2SH_SEGWIT, 10)
        self.nodes[0].sendtoaddress(addr_LEGACY, 20)
        self.nodes[0].sendtoaddress(addr_BECH32, 40)

        #send to child keys of tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK
        self.nodes[0].sendtoaddress("xcQmb895Wa9ztBCnDsjj3RbX5WkRLbLSc5", 80) # (m/0'/0'/0')
        self.nodes[0].sendtoaddress("xawm2Mp5LbXywSDGXiQVsmVDn6mFJSpuwC", 160) # (m/0'/0'/1')
        self.nodes[0].sendtoaddress("xuEukCgsXhWwM3vDiv6TLcb7CtnaUPxGdi", 320) # (m/0'/0'/1500')
        self.nodes[0].sendtoaddress("xhZS1kiuxEqq5f1adCfrRb9EGoqGcxYb4W", 640) # (m/0'/0'/0)
        self.nodes[0].sendtoaddress("xeaxfcYJ1vNFmzLfPduiKcar6iXJU9Bzce", 1280) # (m/0'/0'/1)
        self.nodes[0].sendtoaddress("xcnAnZ8xKLf8jLn3sCpRJgeRHHtBNpGy5q", 2560) # (m/0'/0'/1500)
        self.nodes[0].sendtoaddress("xbGGqvuNMV9EbEaGrj1QUwR5iR8yACdDiX", 5120) # (m/1/1/0')
        self.nodes[0].sendtoaddress("xXucQFT1qmJNfVzMNrtxT2jeTfLx5EpB7w", 10240) # (m/1/1/1')
        self.nodes[0].sendtoaddress("xg2PC74XkFXST17MYBy7BvnBnF2LsteG1x", 20480) # (m/1/1/1500')
        self.nodes[0].sendtoaddress("xknmPQdjKCaZ5DvmVG1QYfJkyRmA2a1nry", 40960) # (m/1/1/0)
        self.nodes[0].sendtoaddress("xpwQX3mubsgVB2tnYm1zvtH8sgEkBADpwE", 81920) # (m/1/1/1)
        self.nodes[0].sendtoaddress("xgXRSjnwsgX7Fb4k5REhYxq1inUDLQmk7Q", 163840) # (m/1/1/1500)


        self.nodes[0].generate(1)

        self.log.info("Stop node, remove wallet, mine again some blocks...")
        self.stop_node(0)
        shutil.rmtree(os.path.join(self.nodes[0].datadir, "regtest", 'wallets'))
        self.start_node(0)
        self.nodes[0].generate(110)

        self.restart_node(0, ['-nowallet'])
        self.log.info("Test if we have found the non HD unspent outputs.")
        assert_equal(self.nodes[0].scantxoutset("start", [ "pkh(" + pubk1 + ")", "pkh(" + pubk2 + ")", "pkh(" + pubk3 + ")"])['total_amount'], Decimal("20.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "wpkh(" + pubk1 + ")", "wpkh(" + pubk2 + ")", "wpkh(" + pubk3 + ")"])['total_amount'], Decimal("40.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "sh(wpkh(" + pubk1 + "))", "sh(wpkh(" + pubk2 + "))", "sh(wpkh(" + pubk3 + "))"])['total_amount'], Decimal("10.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(" + pubk1 + ")", "combo(" + pubk2 + ")", "combo(" + pubk3 + ")"])['total_amount'], Decimal("70.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "addr(" + addr_P2SH_SEGWIT + ")", "addr(" + addr_LEGACY + ")", "addr(" + addr_BECH32 + ")"])['total_amount'], Decimal("70.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "addr(" + addr_P2SH_SEGWIT + ")", "addr(" + addr_LEGACY + ")", "combo(" + pubk3 + ")"])['total_amount'], Decimal("70.0000"))

        self.log.info("Test extended key derivation.")
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/0'/0h/0h)"])['total_amount'], Decimal("80.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/0'/0'/1h)"])['total_amount'], Decimal("160.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/0h/0'/1500')"])['total_amount'], Decimal("320.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/0h/0h/0)"])['total_amount'], Decimal("640.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/0'/0h/1)"])['total_amount'], Decimal("1280.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/0h/0'/1500)"])['total_amount'], Decimal("2560.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ {"desc": "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/0'/0h/*h)", "range": 1499}])['total_amount'], Decimal("240.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ {"desc": "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/0'/0'/*h)", "range": 1500}])['total_amount'], Decimal("560.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ {"desc": "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/0h/0'/*)", "range": 1499}])['total_amount'], Decimal("1920.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ {"desc": "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/0'/0h/*)", "range": 1500}])['total_amount'], Decimal("4480.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/1/1/0')"])['total_amount'], Decimal("5120.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/1/1/1')"])['total_amount'], Decimal("10240.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/1/1/1500h)"])['total_amount'], Decimal("20480.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/1/1/0)"])['total_amount'], Decimal("40960.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/1/1/1)"])['total_amount'], Decimal("81920.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/1/1/1500)"])['total_amount'], Decimal("163840.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tpubD6NzVbkrYhZ4WaWSyoBvQwbpLkojyoTZPRsgXELWz3Popb3qkjcJyJUGLnL4qHHoQvao8ESaAstxYSnhyswJ76uZPStJRJCTKvosUCJZL5B/1/1/0)"])['total_amount'], Decimal("40960.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tpubD6NzVbkrYhZ4WaWSyoBvQwbpLkojyoTZPRsgXELWz3Popb3qkjcJyJUGLnL4qHHoQvao8ESaAstxYSnhyswJ76uZPStJRJCTKvosUCJZL5B/1/1/1)"])['total_amount'], Decimal("81920.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ "combo(tpubD6NzVbkrYhZ4WaWSyoBvQwbpLkojyoTZPRsgXELWz3Popb3qkjcJyJUGLnL4qHHoQvao8ESaAstxYSnhyswJ76uZPStJRJCTKvosUCJZL5B/1/1/1500)"])['total_amount'], Decimal("163840.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ {"desc": "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/1/1/*')", "range": 1499}])['total_amount'], Decimal("15360.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ {"desc": "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/1/1/*')", "range": 1500}])['total_amount'], Decimal("35840.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ {"desc": "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/1/1/*)", "range": 1499}])['total_amount'], Decimal("122880.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ {"desc": "combo(tprv8ZgxMBicQKsPd7Uf69XL1XwhmjHopUGep8GuEiJDZmbQz6o58LninorQAfcKZWARbtRtfnLcJ5MQ2AtHcQJCCRUcMRvmDUjyEmNUWwx8UbK/1/1/*)", "range": 1500}])['total_amount'], Decimal("286720.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ {"desc": "combo(tpubD6NzVbkrYhZ4WaWSyoBvQwbpLkojyoTZPRsgXELWz3Popb3qkjcJyJUGLnL4qHHoQvao8ESaAstxYSnhyswJ76uZPStJRJCTKvosUCJZL5B/1/1/*)", "range": 1499}])['total_amount'], Decimal("122880.0000"))
        assert_equal(self.nodes[0].scantxoutset("start", [ {"desc": "combo(tpubD6NzVbkrYhZ4WaWSyoBvQwbpLkojyoTZPRsgXELWz3Popb3qkjcJyJUGLnL4qHHoQvao8ESaAstxYSnhyswJ76uZPStJRJCTKvosUCJZL5B/1/1/*)", "range": 1500}])['total_amount'], Decimal("286720.0000"))

if __name__ == '__main__':
    ScantxoutsetTest().main()
