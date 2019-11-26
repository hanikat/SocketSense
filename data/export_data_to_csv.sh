DATABASE="${1}"
#SELECT="SELECT * FROM "

FILENAME="${2}.csv"


echo "Exporting from database ${DATABASE} ${SELECT} to ${FILENAME}"
influx -database ${DATABASE} -execute 'SELECT * FROM socket_data' -format csv > ${FILENAME}


#influx -database 'socket_sensedb' -execute 'SELECT * FROM socket_data' -format csv > test.csv




