/*
    Dependencies
        1. libssh-dev
        2. libmysqlclient-dev
    Compile using
        gcc -o query_interface query_interface.c $(mysql_config --cflags --libs) -lssh
    Run using
        ./query_interface
*/

#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>

void run_query(MYSQL *conn, char* query){
    MYSQL_RES *res;
    MYSQL_ROW row;
    if(mysql_query(conn, query)){
        fprintf(stderr, "%s", mysql_error(conn));
        exit(1);
    }
    res = mysql_use_result(conn);
    if(res == NULL){
        fprintf(stderr, "%s", mysql_error(conn));
        exit(1);
    }
    int num_fields = mysql_num_fields(res);
    MYSQL_FIELD *fields = mysql_fetch_fields(res);
    for(int i = 0; i < num_fields; i++){
        printf("%-30s", fields[i].name);
    }
    printf("\n");
    while((row = mysql_fetch_row(res)) != NULL){
        for(int i = 0; i < num_fields; i++){
            printf("%-30s\t", row[i]);
        }
        printf("\n");
    }
    mysql_free_result(res);
    return;
}

void print_interface(){
    printf("The queries that can be executed are:\n");
    printf("1. Names of all physicians who are trained in procedure name “bypass surgery”\n");
    printf("2. Names of all physicians affiliated with the department name “cardiology” and trained in “bypass surgery”\n");
    printf("3. Names of all the nurses who have ever been on call for room 123\n");
    printf("4. Names and addresses of all patients who were prescribed the medication named “remdesivir”\n");
    printf("5. Name and insurance id of all patients who stayed in the “icu” room type for more than 15 days\n");
    printf("6. Names of all nurses who assisted in the procedure name “bypass surgery”\n");
    printf("7. Name and position of all nurses who assisted in the procedure name “bypass surgery” along with the names of and the accompanying physicians\n");
    printf("8. Obtain the names of all physicians who have performed a medical procedure they have never been trained to perform\n");
    printf("9. Names of all physicians who have performed a medical procedure that they are trained to perform, but such that the procedure was done at a date after the physician's certification expired\n");
    printf("10. Physician name, name of procedure, date when the procedure was carried out, name of the patient the procedure was carried out on where the physicial has performed a medical procedure that they are trained to perform, but such that the procedure was done at a date after the physician's certification expired\n");
    printf("11. Names of all patients (also include, for each patient, the name of the patient's physician), such that all the following are true:\n");
    printf("• The patient has been prescribed some medication by his/her physician\n");
    printf("• The patient has undergone a procedure with a cost larger that 5000\n");
    printf("• The patient has had at least two appointment where the physician was affiliated with the cardiology department\n");
    printf("• The patient's physician is not the head of any department\n");
    printf("12. Name and brand of the medication which has been prescribed to the highest number of patients\n");
    printf("13. Name of all the physicians trained in a particular procedure\n");
}

int main(){
    ssh_session my_ssh_session;
    int rc;

    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL)
        exit(-1);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, "10.5.18.72");
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, "20CS30063");

    rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK){
        fprintf(stderr, "Error connecting to server: %s", ssh_get_error(my_ssh_session));
        ssh_free(my_ssh_session);
        exit(-1);
    }
    rc = ssh_userauth_password(my_ssh_session, NULL, "du20");
    if (rc != SSH_AUTH_SUCCESS){
        fprintf(stderr, "Error authenticating with password: %s", ssh_get_error(my_ssh_session));
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        exit(-1);
    }
    printf("Connected to server\n");

    MYSQL *conn;
    char* server = "10.5.18.72";
    char* user = "20CS30063";
    char* password = "20CS30063";
    char* database = "20CS30063";
    int port = 3306;
    conn = mysql_init(NULL);
    if(conn == NULL){
        fprintf(stderr, "%s", mysql_error(conn));
        exit(1);
    }

    if(!mysql_real_connect(conn, server, user, password, database, port, NULL, 0)){
        fprintf(stderr, "%s", mysql_error(conn));
        exit(1);
    }
    printf("Connected to database\n\n");

    while(1){
        print_interface();
        printf("Enter the query number(0 to exit): ");
        int query_num;
        scanf("%d", &query_num);
        if(query_num == 0){
            break;
        }
        char query[1000];
        switch (query_num){
        case 1:
            strcpy(query, "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Trained_In, Procedure_underwent WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Trained_In.Treatment AND Physician.EmployeeID = Trained_In.Physician;");
            run_query(conn, query);
            break;
        case 2:
            strcpy(query, "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Trained_In, Procedure_underwent, Affiliated_with, Department WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Trained_In.Treatment AND Physician.EmployeeID = Trained_In.Physician AND Physician.EmployeeID = Affiliated_with.Physician AND Affiliated_with.Department = Department.DepartmentID AND Department.Name = 'Cardiology';");
            run_query(conn, query);
            break;
        case 3:
            strcpy(query, "SELECT DISTINCT Nurse.Name AS \"Nurse Name\" FROM Nurse, On_Call, Room WHERE Room.Number = 123 AND Room.BlockFloor = On_Call.BlockFloor AND Room.BlockCode = On_Call.BlockCode AND Nurse.EmployeeID = On_Call.Nurse;");
            run_query(conn, query);
            break;
        case 4:
            strcpy(query, "SELECT DISTINCT Patient.Name AS \"Patient Name\", Patient.Address AS \"Patient Address\" FROM Patient, Prescribes, Medication WHERE Medication.Name = 'remdesivir' AND Medication.Code = Prescribes.Medication AND Patient.SSN = Prescribes.Patient;");
            run_query(conn, query);
            break;
        case 5:
            strcpy(query, "SELECT DISTINCT Patient.Name AS \"Patient Name\", Patient.InsuranceID AS \"Insurance ID\" FROM Patient, Stay, Room WHERE Room.Type = 'ICU' AND Room.Number = Stay.Room AND Patient.SSN = Stay.Patient AND extract(day from (Stay.End_time - Stay.Start_time)) > 15;");
            run_query(conn, query);
            break;
        case 6:
            strcpy(query, "SELECT DISTINCT Nurse.Name AS \"Nurse Name\" FROM Nurse, Undergoes, Procedure_underwent WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Nurse.EmployeeID = Undergoes.AssistingNurse;");
            run_query(conn, query);
            break;
        case 7:
            strcpy(query, "SELECT DISTINCT Nurse.Name AS \"Nurse Name\", Nurse.Position AS \"Nurse Position\", Physician.Name AS \"Physician Name\" FROM Nurse, Undergoes, Procedure_underwent, Physician WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Nurse.EmployeeID = Undergoes.AssistingNurse AND Physician.EmployeeID = Undergoes.Physician;");
            run_query(conn, query);
            break;
        case 8:
            strcpy(query, "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Undergoes, Procedure_underwent WHERE Physician.EmployeeID = Undergoes.Physician AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Procedure_underwent.Code NOT IN (SELECT Trained_In.Treatment FROM Trained_In WHERE Trained_In.Physician = Physician.EmployeeID);");
            run_query(conn, query);
            break;
        case 9:
            strcpy(query, "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Undergoes, Procedure_underwent, Trained_In WHERE Physician.EmployeeID = Undergoes.Physician AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Procedure_underwent.Code = Trained_In.Treatment AND Trained_In.Physician = Physician.EmployeeID AND Undergoes.Date > Trained_In.CertificationExpires;");
            run_query(conn, query);
            break;
        case 10:
            strcpy(query, "SELECT DISTINCT Patient.Name AS \"Patient Name\", Physician.Name AS \"Physician Name\" FROM Patient, Prescribes, Physician, Undergoes, Procedure_underwent, Appointment, Affiliated_with, Department WHERE Patient.SSN = Prescribes.Patient AND Prescribes.Physician = Physician.EmployeeID AND Patient.SSN = Undergoes.Patient AND Undergoes.Procedure_underwent = Procedure_underwent.Code AND Patient.SSN = Appointment.Patient AND Appointment.Physician = Affiliated_with.Physician AND Affiliated_with.Department = Department.DepartmentID AND Procedure_underwent.Cost > 5000 AND Department.Name = 'Cardiology' AND Physician.EmployeeID NOT IN (SELECT Head FROM Department) GROUP BY Patient.Name, Physician.Name HAVING COUNT(Appointment.AppointmentID) >= 2;");
            run_query(conn, query);
            break;
        case 11:
            strcpy(query, "SELECT DISTINCT Patient.Name AS \"Patient Name\", Patient.Address AS \"Patient Address\" FROM Patient, Prescribes, Medication WHERE Medication.Name = 'remdesivir' AND Medication.Code = Prescribes.Medication AND Patient.SSN = Prescribes.Patient AND Patient.SSN NOT IN (SELECT Patient FROM Prescribes WHERE Medication IN (SELECT Medication FROM Prescribes GROUP BY Medication HAVING COUNT(*) = (SELECT MAX(Count) FROM (SELECT COUNT(*) AS Count FROM Prescribes GROUP BY Medication) AS MaxCount)));");
            run_query(conn, query);
            break;
        case 12:
            strcpy(query, "SELECT DISTINCT Medication.Name AS \"Medication Name\", Medication.Brand AS \"Medication Brand\" FROM Patient, Prescribes, Medication WHERE Patient.SSN = Prescribes.Patient AND Medication.Code = Prescribes.Medication AND Medication.Code IN (SELECT Medication FROM Prescribes GROUP BY Medication HAVING COUNT(*) = (SELECT MAX(Count) FROM (SELECT COUNT(*) AS Count FROM Prescribes GROUP BY Medication) AS MaxCount));");
            run_query(conn, query);
            break;
        case 13:
            printf("Enter the name of the procedure: ");
            char *procedure_name = (char *)malloc(100);
            scanf("%s", procedure_name);
            strcpy(query, "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Trained_In, Procedure_underwent WHERE Procedure_underwent.Name = \'");
            strcat(query, procedure_name);
            strcat(query, "\' AND Procedure_underwent.Code = Trained_In.Treatment AND Physician.EmployeeID = Trained_In.Physician;");
            run_query(conn, query);
            break;
        default:
            printf("Invalid query number\n");
            break;
        }
        printf("Press any key to continue...\n\n");
        getchar();
        getchar();
    }
    mysql_close(conn);
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    return 0;

}