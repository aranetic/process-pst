# process-pst: Command-line program for turning PSTs into email+loadfiles

Copyright (c) 2010 Aranetic LLC.

process-pst is a command-line tool that does basic, first-pass processing
for PST files.

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

process-pst uses CMake to manage the build process.

### Mac

First, use MacPorts to install Boost 1.42, GCC 4.4, CMake 2.8 and iconv:

    sudo port install boost @1.42.0
    sudo port install gcc44 cmake libiconv

To run the unit tests, you will also want to install Ruby, rubygems, and
bundler.

Then, install the necessary Ruby gems and build using CMake:

    bundle install
    CC=gcc-mp-4.4 CXX=g++-mp-4.4 cmake .
    make

### Linux

These instructions have been tested on a pristine Ubuntu 10.04 system
created using Amazon's EC2 service and the 32-bit ami-2d4aa444.

First, set up your system with the necessary compilers, libraries and gems:

    sudo apt-get install cmake g++-4.4 ruby ruby-dev build-essential \
      libxml2-dev libxslt-dev git-core
    wget http://production.cf.rubygems.org/rubygems/rubygems-1.3.7.tgz
    tar xzf rubygems-1.3.7.tgz
    (cd rubygems-1.3.7 && sudo ruby setup.rb)
    sudo gem1.8 install bundler -v 0.9.26

Boost must be installed manually:

    wget http://downloads.sourceforge.net/project/boost/boost/1.42.0/boost_1_42_0.tar.gz?use_mirror=voxel
    tar xzf boost_1_42_0.tar.gz
    cd boost_1_42_0
    ./bootstrap.sh --prefix=/opt/boost --without-libraries=python
    ./bjam
    sudo ./bjam install
    cd ..

Next, check out process-pst:

    git clone git://github.com/aranetic/process-pst.git
    cd process-pst
    git submodule update --init

Then, install the necessary Ruby gems and build using CMake:

    bundle install
    CC=gcc-4.4 CXX=g++-4.4 cmake -D BOOST_ROOT=/opt/boost .
    make

## Running the unit tests

You can run the unit tests using CMake:

    CTEST_OUTPUT_ON_FAILURE=1 make test

All the tests should pass.
