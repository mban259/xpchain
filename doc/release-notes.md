
XPChain Core version *version* is now available from:

  <https://www.xpchain.io/?loc=lnkwallet>

This is a new minor version release, including new features, various bugfixes
and performance improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/xpc-wg/xpchain/issues>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the
installer (on Windows) or just copy over `/Applications/XPChain-Qt` (on Mac).
or `xpchaind`/`xpchain-qt` (on Linux).

Compatibility
==============

XPChain Core is extensively tested on multiple operating systems using
the Linux kernel, macOS 10.10+, and Windows 7 and newer (Windows XP is not supported).

XPChain Core should also work on most other Unix-like systems but is not
frequently tested on them.

Notable changes
===============

Documentation
-------------

- A new document introduces Bitcoin Core's BIP174
  [Partially-Signed Bitcoin Transactions (PSBT)](https://github.com/bitcoin/bitcoin/blob/0.17/doc/psbt.md)
  interface, which is used to allow multiple programs to collaboratively
  work to create, sign, and broadcast new transactions.  This is useful
  for offline (cold storage) wallets, multisig wallets, coinjoin
  implementations, and many other cases where two or more programs need
  to interact to generate a complete transaction.

EXAMPLE
-------

example item


XPChain *version* change log
------------------

### Consensus

### Mining/Staking

### P2P

### GUI

### RPC

### Miscellaneous

Credits
=======

Thanks to everyone who directly contributed to this release:

As well as everyone that reported bugs & issues through Discord etc.
