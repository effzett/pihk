#!/usr/bin/perl
# bumpversion.pl
# Version 0.1
# changes the version number in several specified files
# if one of them fails nothing is changed
# (c) fz@zenmeister.de
#############################################
use strict;
use warnings;
use POSIX qw(strftime);
use File::Copy;

die "Usage: $0 <maj>.<min>.<rel> \n" if @ARGV != 1;

# bumpversion.pl <x.y.z>
# i.e. bumpversion.pl 2.0.5
my $version = shift;
unless ($version =~ /^\d+\.\d+\.\d+$/){
	print("Argument should be exactly in this format: 1.2.3!\nBye...\n");
	exit;
}

my $retval;
my $date = strftime "%d.%m.%Y", localtime;

# Script is changing follwing files
# (this has to be adapted for a new project)
#####################################################
my $file1 = "./documentation/pihk3.tex";
my $cnt1=0;	# counts the changed lines
my $cnt1expectation=1;	# expected changed lines

my $file2="./app.h";
my $cnt2=0;	# counts the changed lines
my $cnt2expectation=1;	# expected changed lines
#####################################################
# backup of original files
my $ext='.oldversion';
local $^I = $ext;

$retval=0;
$cnt1=0;
local @ARGV = "$file1";
while(<>){
	$retval = s/(^\\newcommand\{\\vnr\}\{)\d+\.\d+\.\d+/$1$version/	;
	if($retval){
		$cnt1++;
	}
	print;
} continue {close ARGV if eof};

$retval=0;
$cnt2=0;
local @ARGV = "$file2";
while(<>){
	$retval = s/(#define APP_VERSION s*\")\d+\.\d+\.\d+/$1$version/ ;
	if($retval){
		$cnt2++;
	}

	print;
} continue {close ARGV if eof};

# check if all ran sucessfully
if($cnt1!=$cnt1expectation  || $cnt2!=$cnt2expectation){	# s.t. is not changed as expected!!
	# rollback all changed files
	print "Something went wrong! Rolling back...\n";
	if(-e "$file1$ext" && -e "$file2$ext"){
		print "All Backup files exists...\n";
		unlink($file1) or die "Could not delete $!\n";
		unlink($file2) or die "Could not delete $!\n";
		print "$file1 deleted.\n";
		print "$file2 deleted.\n";
		move("$file1$ext","$file1") or die "Could not move $!\n";
		move("$file2$ext","$file2") or die "Could not move $!\n";
		print "$file1 recreated.\n";
		print "$file2 recreated.\n";
	}
	print "...done."
}