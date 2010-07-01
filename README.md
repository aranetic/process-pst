# process-pst: Command-line program for turning PSTs into email+loadfiles

Copyright (c) 2010 Aranetic LLC.

This repository will eventually contain a command-line tool which does
basic, first-pass processing for PST files.

    process-pst custodian1.pst custodian1

This will create the directory `custodian1` (if it doesn't already exist),
and an [EDRM XML loadfile][1] `custodian1/edrm-loadfile.xml`.  Any emails
found in `custodian1` will be converted to RFC822 format and stored in
`custodian1`, and any attachments will be extracted.

We are also interested in supporting simple text extraction and other loadfile
formats, including Concordance- and Summation-compatible loadfiles.  Your
patches are extremely welcome!

Note that `process-pst` is distributed under a "share and share alike"
license: If you distribute copies of `process-pst`, you must make the
source code available under the terms of the [GNU Affero General Public
License, version 3][2].  And if you modify `process-pst` and allow other
people to interact with it over a network, you have certain obligations to
provide them with the modified source code.  You may have certain other
obligations.  See the full text of the license for details.  You can think
of this as the vendor version of _pro bono_ work on behalf of the larger
legal community.

[1]: http://edrm.net/2007_2008/xml.php "EDRM XML specifications"
[2]: http://www.gnu.org/licenses/agpl.html

## License

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU Affero General Public License (the "License") as
published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but it is
provided on an "AS-IS" basis and WITHOUT ANY WARRANTY; without even the
implied warranties of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
NONINFRINGEMENT.  See the GNU Affero General Public License for more
details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

## Compiling

Build notes will go here...
