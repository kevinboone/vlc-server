#!/usr/bin/perl -w

# vlc-server-get-stations.pl

# This is a sample Perl script that retrieves the full list of stations from
#   api.radio-browser.info, filters it according to certain criteria, and 
#   writes to stdout a tab-separated station list that is suitable for input to 
#   vlc-server-add-station-list.sh. Of course, you can edit the output file
#   before feeding it into vlc-server-add-station-list.sh, or just build a
#   station list manually. 
# 
# This is not supposed to be a finished piece of softare, but rather an example
#   of how to filter the radio-browser.info data according to personal 
#   preferences. In this case, I filter by location 'United Kingdom', and
#   specific genre tags. 
# 
# In practice, some knowledge of how the output from radio-browser.info is
#   structured is necessary, in order to tweak the script to get useful
#   results. This is why the script does not clean up after itself:
#   it leave the huge XML file from radio-browser.info, so its fields
#   can be inspected, with a view to tweaking the script.
#
# It's generally helpful to process radio station in groups accordinding
#   to some notion of genre, and then write the appropriate genre
#   entries into the database. radio-browser does no use genres, but only
#   tags. vlc-server does not use the tags for anything at present, but
#   they are displayed, along with the location code, in the web user
#   interface. 
#
#   Copyright (c)2023-4 Kevin Boone, GPL3


# Download the full station list from here:
my $url = "http://de1.api.radio-browser.info/xml/stations";

# My basic selection criteria -- UK location, English language
my $countries="United Kingdom";
my $languages="english";

# Keep the station list in a persistent file, in case
#   we need to look at it for information about available tags, etc.
my $tempfile="/tmp/stations";

# 
# store 
# Write a station entry as an SQL INSERT statement. 
#
sub store ($$$$)
  {
  my $name = $_[0];
  my $location = $_[1];
  my $uri = $_[2];
  my $tags = $_[3];
  print "${name}\t${location}\t${uri}\t${tags}\n";
  }

sub decode_entities ($)
  {
  my $in = $_[0];
  $in =~ s/&amp;/\&/g;
  $in =~ s/&apos;/'/g;
  $in =~ s/&quot;/"/g;
  return $in;
  }

sub filter_and_store ($$)
  {
  my $infile = $_[0];
  my $tags = $_[1];

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
	      my $name = decode_entities ($1);
	      $line =~ /url_resolved="(.*?)"/;
	      my $uri = $1;
              my $decoded_taglist = decode_entities (${taglist});
              store (${name}, ${countrycode}, ${uri}, ${decoded_taglist});
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
system ("curl --output $tempfile $url"); 

filter_and_store ($tempfile, "news|drama");
filter_and_store ($tempfile, "oldies");
filter_and_store ($tempfile, "blues"); 
filter_and_store ($tempfile, "rock");
filter_and_store ($tempfile, "classical");

