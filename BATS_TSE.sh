#!/bin/bash
# Script name: BATS_TSE.sh
#
# Description: Full Automated Shell script to build and extensively test the Amazing Project
#
# Input: None, please don't put any
#
# Output: Output is redirected to a "results directory" that contains logfiles for each run
#

directory=results
echo "Starting testing program..."
echo "Building components..."
echo ""
make
echo ""
echo "Build finished"
cd $directory
echo ""
echo "First set of tests check arguments."
echo ""
echo "Test 1: ./AMStartup"
`../AMStartup/AMStartup`
echo ""
echo "Test 2: ./AMStartup 2 stratton.cs.dartmouth.edu"
`../AMStartup/AMStartup 2 stratton.cs.dartmouth.edu`
echo ""
echo "Test 3: ./AMStartup 2 2 nonserver.cs.dartmouth.edu"
`../AMStartup/AMStartup 2 2 nonserver.cs.dartmouth.edu`
echo ""
echo "Test 4: ./AMStartup 2 10 stratton.cs.dartmouth.edu"
`../AMStartup/AMStartup 2 10 stratton.cs.dartmouth.edu`
echo ""
echo "Now unit testing AMStartup..."
echo ""
gcc -o unit_test ../AMStartup/UnitTestAMStartup.c
./unit_test
echo ""
echo "Now unit testing avatar_client..."
echo ""
gcc -o avatar_unit_test ../avatar_client/UnitTestAvatarClient.c
./avatar_unit_test
echo ""
echo "Next set tests functionality."
echo ""
echo "Test 5: ./AMStartup 2 2 stratton.cs.dartmouth.edu"
`../AMStartup/AMStartup 2 2 stratton.cs.dartmouth.edu`
echo ""
echo "Test 6: ./AMStartup 8 2 stratton.cs.dartmouth.edu"
`../AMStartup/AMStartup 8 2 stratton.cs.dartmouth.edu`
echo ""
echo "Test 7: ./AMStartup 4 5 stratton.cs.dartmouth.edu"
`../AMStartup/AMStartup 4 5 stratton.cs.dartmouth.edu`
echo ""
echo "Test 8: ./AMStartup 4 6 stratton.cs.dartmouth.edu"
`../AMStartup/AMStartup 4 6 stratton.cs.dartmouth.edu`
echo ""
echo "Test 9: ./AMStartup 4 8 stratton.cs.dartmouth.edu"
`../AMStartup/AMStartup 4 8 stratton.cs.dartmouth.edu`
echo ""
echo "Test 10: ./AMStartup 8 8 stratton.cs.dartmouth.edu"
`../AMStartup/AMStartup 8 8 stratton.cs.dartmouth.edu`
echo ""
echo "Testing finished."
echo ""
cd ..
make clean
#end
