#!/usr/bin/perl -w

# vlc-server-add-station-list.pl

#   Copyright (c)2023 Kevin Boone, GPL3

use strict;
use warnings;

# The file for generated SQL. This also won't be cleaned, because we
#   may need to look at if for troubleshooting.
my $sqlfile = "/tmp/stations.sql";
 
# If sqlite3 is not on the $PATH, define its path here.
my $sqlite3_prog = "sqlite3";

# 
# escape_sql
# Covert ' to '', as SQL requires
#
sub escape_sql ($)
  {
  my $s = $_[0];
  $s =~ s/\'/\'\'/g;
  return $s;
  }

# 
# store 
# Write a station entry as an SQL INSERT statement. 
#
sub store ($$$$$)
  {
  my $SQLFILE = shift; 
  my $name = $_[0];
  my $genre = $_[1];
  my $album = $_[2];
  my $path = $_[3];
  my $sql = "insert into files (path, title, genre, album) values (" 
     . "'=" . escape_sql (${path}) . "', " 
     . "'" . escape_sql (${name}) . "', " 
     . "'" . escape_sql (${genre}) . "', " 
     . "'" . escape_sql (${album}) . "'" 
     . ");\n";
  print $SQLFILE "${sql}\n";
  }


# 
# START HERE
#
if ($ARGV[0] and $ARGV[1])
  {
  my $tsvfile = $ARGV[0];
  my $database = $ARGV[1];

  open SQLFILE, ">${sqlfile}" or die "Can't open ${sqlfile}\n";
  open TSVFILE, "<${tsvfile}" or die "Can't open ${tsvfile}\n";

  printf SQLFILE "delete from files where album like '%%radio station%%';\n";

  while (my $line = <TSVFILE>) 
    {
    chomp ($line);

    my @tokens = split ('\t', $line);
    if (scalar (@tokens) == 3)
      {
      my $station_name = "Radio station - " . $tokens[0] ;
      my $station_path = $tokens[1];
      my $station_genre = $tokens[2];
      
      store (\*SQLFILE, $station_name, $station_genre, 
        $station_name, $station_path);
      } 
    }

  close (TSVFILE);
  close (SQLFILE);

  system (${sqlite3_prog} . " \"" . ${database} . "\" < " . ${sqlfile});
  }
else
  {
  printf ("Usage: vlc-server-add-station-list.pl {list.tsv} {database.sqlite}\n");
  exit(0);
  }
exit(0);
