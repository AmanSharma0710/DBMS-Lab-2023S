'''
    Dependencies:
        1. mysql.connector
        2. paramiko
        3. prettytable
    Usage:
        Run the script using python3 query_interface.py
'''

import mysql.connector
import paramiko
from datetime import datetime
from prettytable import PrettyTable

# Server credentials
server_ip = "10.5.18.72"
server_username = "20CS30063"
server_password = "du20"
# Database credentials
db_name = "20CS30063"
user_name = "20CS30063"
user_password = "20CS30063"
db_port = 3306

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())

#Connect to the postgresql server
def connect_to_db():
    global conn
    try:
        ssh.connect(server_ip, username=server_username, password=server_password)
        print("Connected to the server")
    except:
        print("Error connecting to the server")
        exit(1)
    try:
        conn = mysql.connector.connect(
            database=db_name,
            user=user_name,
            password=user_password,
            host=server_ip,
            port=db_port
        )
        print("Connected to the database")
    except:
        print("Error connecting to the database")
        exit(1)


#Close the connection to the postgresql server
def close_conn():
    global conn
    conn.close()
    print("Connection closed")


#Execute a query and return the result
def execute_query(query):
    global conn
    cursor_obj = conn.cursor()
    cursor_obj.execute(query)
    columns = [column[0] for column in cursor_obj.description]
    result = cursor_obj.fetchall()
    return columns, result



#Function to print the interface
def print_interface():
    print("The queries that can be executed are:")
    print("1. Names of all physicians who are trained in procedure name “bypass surgery”")
    print("2. Names of all physicians affiliated with the department name “cardiology” and trained in “bypass surgery”")
    print("3. Names of all the nurses who have ever been on call for room 123")
    print("4. Names and addresses of all patients who were prescribed the medication named “remdesivir”")
    print("5. Name and insurance id of all patients who stayed in the “icu” room type for more than 15 days")
    print("6. Names of all nurses who assisted in the procedure name “bypass surgery”")
    print("7. Name and position of all nurses who assisted in the procedure name “bypass surgery” along with the names of and the accompanying physicians")
    print("8. Obtain the names of all physicians who have performed a medical procedure they have never been trained to perform")
    print("9. Names of all physicians who have performed a medical procedure that they are trained to perform, but such that the procedure was done at a date after the physician's certification expired")
    print("10. Physician name, name of procedure, date when the procedure was carried out, name of the patient the procedure was carried out on where the physicial has performed a medical procedure that they are trained to perform, but such that the procedure was done at a date after the physician's certification expired")
    print("11. Names of all patients (also include, for each patient, the name of the patient's physician), such that all the following are true:")
    print("• The patient has been prescribed some medication by his/her physician")
    print("• The patient has undergone a procedure with a cost larger that 5000")
    print("• The patient has had at least two appointment where the physician was affiliated with the cardiology department")
    print("• The patient's physician is not the head of any department")
    print("12. Name and brand of the medication which has been prescribed to the highest number of patients")
    print("13. Name of all the physicians trained in a particular procedure")
    return

#List of queries that can be executed
query_list = [
    "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Trained_In, Procedure_underwent WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Trained_In.Treatment AND Physician.EmployeeID = Trained_In.Physician;",
    "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Trained_In, Procedure_underwent, Affiliated_with, Department WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Trained_In.Treatment AND Physician.EmployeeID = Trained_In.Physician AND Physician.EmployeeID = Affiliated_with.Physician AND Affiliated_with.Department = Department.DepartmentID AND Department.Name = 'Cardiology';",
    "SELECT DISTINCT Nurse.Name AS \"Nurse Name\" FROM Nurse, On_Call, Room WHERE Room.Number = 123 AND Room.BlockFloor = On_Call.BlockFloor AND Room.BlockCode = On_Call.BlockCode AND Nurse.EmployeeID = On_Call.Nurse;",
    "SELECT DISTINCT Patient.Name AS \"Patient Name\", Patient.Address AS \"Patient Address\" FROM Patient, Prescribes, Medication WHERE Medication.Name = 'remdesivir' AND Medication.Code = Prescribes.Medication AND Patient.SSN = Prescribes.Patient;",
    "SELECT DISTINCT Patient.Name AS \"Patient Name\", Patient.InsuranceID AS \"Insurance ID\" FROM Patient, Stay, Room WHERE Room.Type = 'ICU' AND Room.Number = Stay.Room AND Patient.SSN = Stay.Patient AND extract(day from (Stay.End_time - Stay.Start_time)) > 15;",
    "SELECT DISTINCT Nurse.Name AS \"Nurse Name\" FROM Nurse, Undergoes, Procedure_underwent WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Nurse.EmployeeID = Undergoes.AssistingNurse;",
    "SELECT DISTINCT Nurse.Name AS \"Nurse Name\", Nurse.Position AS \"Nurse Position\", Physician.Name AS \"Physician Name\" FROM Nurse, Undergoes, Procedure_underwent, Physician WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Nurse.EmployeeID = Undergoes.AssistingNurse AND Physician.EmployeeID = Undergoes.Physician;",
    "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Undergoes, Procedure_underwent WHERE Physician.EmployeeID = Undergoes.Physician AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Procedure_underwent.Code NOT IN (SELECT Trained_In.Treatment FROM Trained_In WHERE Trained_In.Physician = Physician.EmployeeID);",
    "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Undergoes, Procedure_underwent, Trained_In WHERE Physician.EmployeeID = Undergoes.Physician AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Procedure_underwent.Code = Trained_In.Treatment AND Trained_In.Physician = Physician.EmployeeID AND Undergoes.Date > Trained_In.CertificationExpires;",
    "SELECT DISTINCT Physician.Name AS \"Physician Name\", Procedure_underwent.Name AS \"Procedure_underwent Name\", Undergoes.Date AS \"Date of Procedure_underwent\", Patient.Name AS \"Patient Name\" FROM Physician, Undergoes, Procedure_underwent, Trained_In, Patient WHERE Physician.EmployeeID = Undergoes.Physician AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Procedure_underwent.Code = Trained_In.Treatment AND Trained_In.Physician = Physician.EmployeeID AND Undergoes.Date > Trained_In.CertificationExpires AND Patient.SSN = Undergoes.Patient;",
    "SELECT DISTINCT Patient.Name AS \"Patient Name\", Physician.Name AS \"Physician Name\" FROM Patient, Prescribes, Physician, Undergoes, Procedure_underwent, Appointment, Affiliated_with, Department WHERE Patient.SSN = Prescribes.Patient AND Prescribes.Physician = Physician.EmployeeID AND Patient.SSN = Undergoes.Patient AND Undergoes.Procedure_underwent = Procedure_underwent.Code AND Patient.SSN = Appointment.Patient AND Appointment.Physician = Affiliated_with.Physician AND Affiliated_with.Department = Department.DepartmentID AND Procedure_underwent.Cost > 5000 AND Department.Name = 'Cardiology' AND Physician.EmployeeID NOT IN (SELECT Head FROM Department) GROUP BY Patient.Name, Physician.Name HAVING COUNT(Appointment.AppointmentID) >= 2;",
    "SELECT DISTINCT Medication.Name AS \"Medication Name\", Medication.Brand AS \"Medication Brand\" FROM Patient, Prescribes, Medication WHERE Patient.SSN = Prescribes.Patient AND Medication.Code = Prescribes.Medication AND Medication.Code IN (SELECT Medication FROM Prescribes GROUP BY Medication HAVING COUNT(*) = (SELECT MAX(Count) FROM (SELECT COUNT(*) AS Count FROM Prescribes GROUP BY Medication) AS MaxCount));"
]

#Function to print the result of a query as a table
def print_result(columns, result):
    table = PrettyTable(columns)
    for row in result:
        table.add_row(row)
    print(table)
    


if __name__=="__main__":
    connect_to_db()
    while True:
        print_interface()
        query_num = int(input("Enter the query number(0 to exit): "))
        if query_num == 0:
            break
        elif query_num < 0 or query_num > 13:
            print("Invalid query number")
            continue
        if(query_num == 13):
            print("Enter the procedure name: ")
            procedure_name = input()
            columns, result = execute_query("SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Trained_In, Procedure_underwent WHERE Procedure_underwent.Name = \'" + procedure_name + "\' AND Procedure_underwent.Code = Trained_In.Treatment AND Physician.EmployeeID = Trained_In.Physician;")
        else:
            query = query_list[query_num-1]
            columns, result = execute_query(query)
        print_result(columns, result)
        #Wait for user to press enter
        input("Press enter to continue")
        print("\n\n")
    close_conn()