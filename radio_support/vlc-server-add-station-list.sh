# Reads a tab-separated station list (like stations.tsv) into an SQL
#   file that can be fed into sqlite3.
# This script is rather like vlc-server-add-stations.pl, except that it
#   does not need perl, which may not be available in embedded systems.
#
# Usage: ./vlc-server-add-stations.sh {stations.tsv} {output_sql_file}
# 
# Copyright (c)Kevin Boone, 2024; GPL v3.0

escape_sql()
  {
  S=$1
  echo ${S/\'/\'\'} 
  }

usage_and_exit()
  {
  echo Usage: "vlc-server-add-station-list.sh {list_file} {sqlite3_file}"
  exit
  }

LIST_FILENAME=$1
SQL_FILENAME=$2

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

if [ -e "$SQL_FILENAME" ]
then
  echo "SQL file already exists"
  usage_and_exit
fi

echo "delete from files where path like '=%';" > $SQL_FILENAME

while IFS= read -r line
do
  #echo "$line"
  IFS=$'\t' read -a array <<< "$line"

  if [ "${#array[@]}" -eq 3 ] ; then
    NAME=${array[0]}
    NAME="Radio station - $NAME"
    PATH=${array[1]}
    PATH="=$PATH"
    GENRE=${array[2]}
    ALBUM=$NAME

    ESC_NAME=$(escape_sql "$NAME")
    ESC_GENRE=$(escape_sql "$GENRE")
    ESC_ALBUM=$(escape_sql "$ALBUM")
    ESC_PATH=$(escape_sql "$PATH")

    SQL=`printf "insert into files (path, title, genre, album) values ('%s', '%s', '%s', '%s');" "$ESC_PATH" "$ESC_NAME" "$ESC_GENRE" "$ESC_ALBUM"`

    echo $SQL >> $SQL_FILENAME
  fi

done < "$LIST_FILENAME"


