#!/usr/bin/perl -w

# vlc-server-add-stations.pl

# This is a sample Perl script that retrieves the full list of stations from
#   api.radio-browser.info, filters it according to certain criteria, and 
#   writes a file of SQL statements that can be fed into the vlc-server media 
#   database. 
# 
# This is not supposed to be a finished piece of softare, but rather an example
#   of how to filter the radio-browser.info data according to personal 
#   preferences. 
# 
# In practice, some knowledge of how the output from radio-browser.info is
#   structured is necessary, in order to tweak the script to get useful
#   results. This is why the script does not clean up after itself:
#   it leave the huge XML file from radio-browser.info, so its fields
#   can be inspected, with a view to tweaking the script.
#
# It's generally helpful to process radio station in groups accorinding
#   to some notion of genre, and then write the appropriate genre
#   entries into the database. radio-browser does no use genres, but only
#   tags. The tags could be probably be used directly as genre entries,
#   but I prefer to use genre entries that match the genres I use for
#   my local audio files. 
#
# This script depends on having sqlite3 and curl installed.
# It takes one argument: the path to the vlc-server media database.
# There's no need to shut down the vlc-server server when running this
#   script.
#
#   Copyright (c)2023 Kevin Boone, GPL3


# Download the full station list from here:
my $url = "http://de1.api.radio-browser.info/xml/stations";

# My basic selection criteria -- UK location, English language
my $countries="United Kingdom";
my $languages="english";

# Keep the station list in a persistent file, in case
#   we need to look at it for information about available tags, etc.
my $tempfile="/tmp/stations";

# The file for generated SQL. This also won't be cleaned, because we
#   may need to look at if for troubleshooting.
my $sqlfile="/tmp/stations.sql";

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

# The filter_and_store() function takes five arguments: 
#   - the handle of the SQL output file
#   - the name of the input file (the complete
#     station list in XML format from radiobrowser.info, 
#   - a regular expression that matches tags in the station entries, 
#   - an "album" name, that will be written as the album tag in the
#     vlc-server database
#   - genre, which will be written as the genre name in the database.
# All these names are arbitrary: they should ideally match the tagging
#   scheme used for the local audio files, so radio stations turn up 
#   in searches of the media database

sub filter_and_store ($$$$$)
  {
  my $SQLFILE = shift; 
  my $infile = $_[0];
  my $tags = $_[1];
  my $album = $_[2];
  my $genre = $_[3];

  open IN, "<$infile" or die "Can't open $infile";

  while (<IN>)
    {
    my $line = $_;
    if ($line) 
      {
      # Only include stations marked OK
      if ($line =~ /lastcheckok="1"/)
	{
	$line =~ /language="(.*?)"/;
	my $language = $1;
	if (!$language) { $language="***"; }
	$language =~ s/&amp;/\&/g;
	$language =~ s/&apos;/'/g;

	$line =~ /country="(.*?)"/;
	my $country = $1;
	if (!$country) { $country="***"; }
	$country =~ s/&amp;/\&/g;
	$country =~ s/&apos;/'/g;

	$line =~ /countrycode="(.*?)"/;
	my $countrycode = $1;
	if (!$countrycode) { $countrycode="?"; }

	# See if the language or country match
	if (($language eq $languages) and ($country =~ /$countries/))
	  {
	  # If country or language matches, see if tags match
	  $line =~ /tags="(.*?)"/;
	  my $taglist = $1;
	  if ($taglist)
	    {
	    if ($taglist =~ /$tags/)
	      {
	      $line =~ /name="(.*?)"/;
	      my $name = $1;
	      $name =~ s/&amp;/\&/g;
	      $name =~ s/&apos;/'/g;
	      $name =~ s/&quot;/"/g;
	      $line =~ /url_resolved="(.*?)"/;
	      my $url = $1;
              store (\*SQLFILE, ${name}, ${genre}, ${album}, ${url});
	      }
	    }
	  } 
	}
      }
    }

  close IN;
  }


# 
# START HERE
#
if ($ARGV[0])
  {
  $database = $ARGV[0];
  print ("Downloading full station list (may take a while)\n");
  system ("curl --output $tempfile $url"); 


  open SQLFILE, ">${sqlfile}" or die "Can't open ${sqlfile}\n";
  printf SQLFILE "delete from files where album like '%%radio stations%%';\n";

  print ("Generating SQL for 'news|drama;\n");
  filter_and_store (\*SQLFILE, $tempfile, 
		"news|drama",                    # Tag(s) to match in RB.info 
		"News and drama radio stations", # "Album" name to generate
		"radio - news and drama");       # genre name to generate
  print ("Generating SQL for 'oldies'\n");
  filter_and_store (\*SQLFILE, $tempfile, 
		"oldies", 
		"Oldies radio stations", 
		"music - oldies");
  # The 'public radio' tag is rarely used by UK stations; it's more
  #   relevant for North America.
  #print ("Generating SQL for 'public radio'\n");
  #filter_and_store (\*SQLFILE, $tempfile, 
  #              "public radio", 
  #              "Public radio stations", 
  #              "radio - public radio");
  #print ("Generating SQL for 'blues'\n");
  filter_and_store (\*SQLFILE, $tempfile, 
                "blues", 
                "Blues radio stations", 
                "music - blues");
  print ("Generating SQL for 'rock'\n");
  filter_and_store (\*SQLFILE, $tempfile,  
                "rock", 
                "Rock radio stations",  
                "music - mainstream rock and pop");
  print ("Generating SQL for 'classical'\n");
  filter_and_store (\*SQLFILE, $tempfile,  
                "classical", 
                "Classical radio stations",  
                "music - general classical");

  close (SQLFILE);

  print ("Using ${sqlite3_prog} to execute SQL\n");
  system (${sqlite3_prog} . " \"" . ${database} . "\" < " . ${sqlfile});
  }
else
  {
  printf ("Usage: vlc-server-add-stations.pl {database.sqlite}\n");
  exit(0);
  }
exit(0);
