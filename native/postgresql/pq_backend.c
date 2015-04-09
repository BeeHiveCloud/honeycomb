#include "pq_backend.h"

int git_pq_init(PGconn **out,const char *conninfo){
	
	PGconn *conn;
	PGresult* stmt;

	conn = PQconnectdb(conninfo);

	
	if (PQstatus(conn) != CONNECTION_OK){
		printf("Connection to database failed: %s",
			PQerrorMessage(conn));
		PQfinish(conn);
		return -1;
	}
	
	stmt = PQprepare(conn,
			         "ODB_READ", 
			         "SELECT TYPE, SIZE, DATA FROM GIT_ODB WHERE OID = $1::VARCHAR(40)", 
			         1, 
					 NULL);
	if (PQresultStatus(stmt) != PGRES_COMMAND_OK){
		printf("Prepare statement failed: %s",
			PQresultErrorMessage(stmt));
		return -1;
	}

	stmt = PQprepare(conn,
					"ODB_READ_HEADER",
					"SELECT TYPE FROM GIT_ODB WHERE OID = $1::VARCHAR(40)",
					1,
					NULL);
	if (PQresultStatus(stmt) != PGRES_COMMAND_OK){
		printf("Prepare statement failed: %s",
			PQresultErrorMessage(stmt));
		return -1;
	}

	stmt = PQprepare(conn,
					"ODB_WRITE",
					"INSERT INTO GIT_ODB VALUES($1::VARCHAR(40),$2::SMALLINT,$3::BIGINT,$4::BYTEA)",
					3,
					NULL);
	if (PQresultStatus(stmt) != PGRES_COMMAND_OK){
		printf("Prepare statement failed: %s",
			PQresultErrorMessage(stmt));
		return NULL;
	}
	
	*out = conn;

	return 0;
}

int git_pq_free(PGconn *conn){
	PQfinish(conn);
	return 0;
}
