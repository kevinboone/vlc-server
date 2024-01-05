# Reads a tab-separated station list (like stations.tsv) into an SQL
#   file and feeds it into sqlite3. 
#
# It is expected that the sqlite3 output file is the vlc-server media
#   database. The station list can be generated using a script like
#   vlc-server-get-stations.pl, or just built manually using a text
#   editor.
#
# Usage: ./vlc-server-add-stations.sh {stations.tsv} {sqlite3_file}
# 
# Copyright (c)Kevin Boone, 2024; GPL v3.0

escape_sql()
  {
  S=$1
  echo ${S/\'/\'\'} 
  }

usage_and_exit()
  {
  echo Usage: "vlc-server-add-station-list2.sh {list_file} {sqlite3_file}"
  exit
  }

LIST_FILENAME=$1
SQLITE_FILENAME=$2
SQL_FILENAME=/tmp/$$.sql

if [ -z "$1" ]
then
  usage_and_exit
fi

if [ -z "$2" ]
then
  usage_and_exit
fi

if [ ! -e "$LIST_FILENAME" ]
then
  echo "No file $LIST_FILENAME"
  usage_and_exit
fi

if [ ! -e "$SQLITE_FILENAME" ]
then
  echo "No file $SQLITE_FILENAME"
  usage_and_exit
fi

echo "delete from streams;" > $SQL_FILENAME

while IFS= read -r line
do
  #echo "$line"
  IFS=$'\t' read -a array <<< "$line"

  if [ "${#array[@]}" -eq 4 ] ; then
    NAME=${array[0]}
    LOCATION=${array[1]}
    URI=${array[2]}
    TAGS=${array[3]}

    ESC_NAME=$(escape_sql "$NAME")
    ESC_LOCATION=$(escape_sql "$LOCATION")
    ESC_TAGS=$(escape_sql "$TAGS")
    ESC_URI=$(escape_sql "$URI")

    SQL=`printf "insert into streams (name, location, tags, uri) values ('%s', '%s', '%s', '%s');" "$ESC_NAME" "$ESC_LOCATION" "$ESC_TAGS" "$ESC_URI"`

    echo $SQL >> $SQL_FILENAME
  fi

done < "$LIST_FILENAME"

sqlite3 "$SQLITE_FILENAME" < "$SQL_FILENAME"

