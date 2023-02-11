/*
    Dependencies:
        1. mysql-connector-java-8.0.23.jar
        2. jsch-0.1.55.jar
    Use the following command to run the program:
        java -cp mysql-connector-java-8.0.23.jar:jsch-0.1.55.jar query_interface.java

*/

import java.util.Scanner;
import java.sql.*;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.Session;

class query_interface{  
    public static void ssh_tunnel() throws Exception{
        String host = "10.5.18.72";
        String username = "20CS30063";
        String password = "du20";
        int port = 22;
        int lport = 5656;
        try{
            JSch jsch = new JSch();
            Session session = jsch.getSession(username, host, port);
            session.setPassword(password);
            session.setConfig("StrictHostKeyChecking", "no");
            System.out.println("Establishing Connection...");
            session.connect();
            int assigned_port = session.setPortForwardingL(lport, "localhost", 3306);
            System.out.println("localhost:" + assigned_port + " -> " + host + ":" + 3306);
            System.out.println("Connection established");
        }
        catch(Exception e){
            System.err.print(e);
        }
    }

    public static void print_interface(){
        System.out.println("The queries that can be executed are:");
        System.out.println("1. Names of all physicians who are trained in procedure name “bypass surgery”");
        System.out.println("2. Names of all physicians affiliated with the department name “cardiology” and trained in “bypass surgery”");
        System.out.println("3. Names of all the nurses who have ever been on call for room 123");
        System.out.println("4. Names and addresses of all patients who were prescribed the medication named “remdesivir”");
        System.out.println("5. Name and insurance id of all patients who stayed in the “icu” room type for more than 15 days");
        System.out.println("6. Names of all nurses who assisted in the procedure name “bypass surgery”");
        System.out.println("7. Name and position of all nurses who assisted in the procedure name “bypass surgery” along with the names of and the accompanying physicians");
        System.out.println("8. Obtain the names of all physicians who have performed a medical procedure they have never been trained to perform");
        System.out.println("9. Names of all physicians who have performed a medical procedure that they are trained to perform, but such that the procedure was done at a date after the physician's certification expired");
        System.out.println("10. Physician name, name of procedure, date when the procedure was carried out, name of the patient the procedure was carried out on where the physicial has performed a medical procedure that they are trained to perform, but such that the procedure was done at a date after the physician's certification expired");
        System.out.println("11. Names of all patients (also include, for each patient, the name of the patient's physician), such that all the following are true:");
        System.out.println("• The patient has been prescribed some medication by his/her physician");
        System.out.println("• The patient has undergone a procedure with a cost larger that 5000");
        System.out.println("• The patient has had at least two appointment where the physician was affiliated with the cardiology department");
        System.out.println("• The patient's physician is not the head of any department");
        System.out.println("12. Name and brand of the medication which has been prescribed to the highest number of patients");
        System.out.println("13. Name of all the physicians trained in a particular procedure");
        return;
    }

    public static void run_query(Connection con, String query){
        try{
            Statement stmt = con.createStatement();
            ResultSet rs = stmt.executeQuery(query);
            ResultSetMetaData rsmd = rs.getMetaData();
            int columnsNumber = rsmd.getColumnCount();
            //Print the column names
            for (int i = 1; i <= columnsNumber; i++) {
                if (i > 1) System.out.print(",  ");
                String columnValue = rsmd.getColumnName(i);
                System.out.print(columnValue);
            }
            System.out.println("");
            while (rs.next()) {
                for (int i = 1; i <= columnsNumber; i++) {
                    if (i > 1) System.out.print(",  ");
                    String columnValue = rs.getString(rsmd.getColumnName(i));
                    System.out.print(columnValue);
                }
                System.out.println("");
            }
            rs.close();
            stmt.close();
        }
        catch(Exception e){
            System.out.println(e);
        }
        return;
    }
        
    public static void main(String args[]){  
        try{
            //We use an ssh tunnel to connect to the system running the database
            ssh_tunnel();
        }
        catch(Exception e){
            e.printStackTrace();
        }
        Connection con = null;
        try{
            //We connect to the database using the ssh tunnel
            Class.forName("com.mysql.cj.jdbc.Driver");
            con = DriverManager.getConnection("jdbc:mysql://localhost:5656/20CS30063", "20CS30063", "20CS30063");
            System.out.println("Database Connected");
        }
        catch(Exception e){
            System.out.println(e);
        }
        Scanner sc = new Scanner(System.in);
        while(true){
            print_interface();
            System.out.print("Enter the number of the query you want to run (0 to exit): ");
            int query_num;
            query_num = sc.nextInt();
            sc.nextLine();
            if(query_num == 0){
                System.out.println("Exiting...");
                break;
            }
            String query = "";
            switch (query_num){
            case 1:
                query = "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Trained_In, Procedure_underwent WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Trained_In.Treatment AND Physician.EmployeeID = Trained_In.Physician;";
                run_query(con, query);
                break;
            case 2:
                query = "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Trained_In, Procedure_underwent, Affiliated_with, Department WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Trained_In.Treatment AND Physician.EmployeeID = Trained_In.Physician AND Physician.EmployeeID = Affiliated_with.Physician AND Affiliated_with.Department = Department.DepartmentID AND Department.Name = 'Cardiology';";
                run_query(con, query);
                break;
            case 3:
                query = "SELECT DISTINCT Nurse.Name AS \"Nurse Name\" FROM Nurse, On_Call, Room WHERE Room.Number = 123 AND Room.BlockFloor = On_Call.BlockFloor AND Room.BlockCode = On_Call.BlockCode AND Nurse.EmployeeID = On_Call.Nurse;";
                run_query(con, query);
                break;
            case 4:
                query = "SELECT DISTINCT Patient.Name AS \"Patient Name\", Patient.Address AS \"Patient Address\" FROM Patient, Prescribes, Medication WHERE Medication.Name = 'remdesivir' AND Medication.Code = Prescribes.Medication AND Patient.SSN = Prescribes.Patient;";
                run_query(con, query);
                break;
            case 5:
                query = "SELECT DISTINCT Patient.Name AS \"Patient Name\", Patient.InsuranceID AS \"Insurance ID\" FROM Patient, Stay, Room WHERE Room.Type = 'ICU' AND Room.Number = Stay.Room AND Patient.SSN = Stay.Patient AND extract(day from (Stay.End_time - Stay.Start_time)) > 15;";
                run_query(con, query);
                break;
            case 6:
                query = "SELECT DISTINCT Nurse.Name AS \"Nurse Name\" FROM Nurse, Undergoes, Procedure_underwent WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Nurse.EmployeeID = Undergoes.AssistingNurse;";
                run_query(con, query);
                break;
            case 7:
                query = "SELECT DISTINCT Nurse.Name AS \"Nurse Name\", Nurse.Position AS \"Nurse Position\", Physician.Name AS \"Physician Name\" FROM Nurse, Undergoes, Procedure_underwent, Physician WHERE Procedure_underwent.Name = 'bypass surgery' AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Nurse.EmployeeID = Undergoes.AssistingNurse AND Physician.EmployeeID = Undergoes.Physician;";
                run_query(con, query);
                break;
            case 8:
                query = "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Undergoes, Procedure_underwent WHERE Physician.EmployeeID = Undergoes.Physician AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Procedure_underwent.Code NOT IN (SELECT Trained_In.Treatment FROM Trained_In WHERE Trained_In.Physician = Physician.EmployeeID);";
                run_query(con, query);
                break;
            case 9:
                query = "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Undergoes, Procedure_underwent, Trained_In WHERE Physician.EmployeeID = Undergoes.Physician AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Procedure_underwent.Code = Trained_In.Treatment AND Trained_In.Physician = Physician.EmployeeID AND Undergoes.Date > Trained_In.CertificationExpires;";
                run_query(con, query);
                break;
            case 10:
                query = "SELECT DISTINCT Physician.Name AS \"Physician Name\", Procedure_underwent.Name AS \"Procedure_underwent Name\", Undergoes.Date AS \"Date of Procedure_underwent\", Patient.Name AS \"Patient Name\" FROM Physician, Undergoes, Procedure_underwent, Trained_In, Patient WHERE Physician.EmployeeID = Undergoes.Physician AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Procedure_underwent.Code = Trained_In.Treatment AND Trained_In.Physician = Physician.EmployeeID AND Undergoes.Date > Trained_In.CertificationExpires AND Patient.SSN = Undergoes.Patient;";
                run_query(con, query);
                break;
            case 11:
                query = "SELECT DISTINCT Patient.Name AS \"Patient Name\", Physician.Name AS \"Physician Name\" FROM Patient, Prescribes, Physician, Undergoes, Procedure_underwent, Appointment, Affiliated_with, Department WHERE Patient.SSN = Prescribes.Patient AND Prescribes.Physician = Physician.EmployeeID AND Patient.SSN = Undergoes.Patient AND Undergoes.Procedure_underwent = Procedure_underwent.Code AND Patient.SSN = Appointment.Patient AND Appointment.Physician = Affiliated_with.Physician AND Affiliated_with.Department = Department.DepartmentID AND Procedure_underwent.Cost > 5000 AND Department.Name = 'Cardiology' AND Physician.EmployeeID NOT IN (SELECT Head FROM Department) GROUP BY Patient.Name, Physician.Name HAVING COUNT(Appointment.AppointmentID) >= 2;";
                run_query(con, query);
                break;
            case 12:
                query = "SELECT DISTINCT Medication.Name AS \"Medication Name\", Medication.Brand AS \"Medication Brand\" FROM Patient, Prescribes, Medication WHERE Patient.SSN = Prescribes.Patient AND Medication.Code = Prescribes.Medication AND Medication.Code IN (SELECT Medication FROM Prescribes GROUP BY Medication HAVING COUNT(*) = (SELECT MAX(Count) FROM (SELECT COUNT(*) AS Count FROM Prescribes GROUP BY Medication) AS MaxCount));";
                run_query(con, query);
                break;
            case 13:
                System.out.print("Enter the name of the procedure: ");
                String procedure_name = sc.nextLine();
                query = "SELECT DISTINCT Physician.Name AS \"Physician Name\" FROM Physician, Undergoes, Procedure_underwent WHERE Physician.EmployeeID = Undergoes.Physician AND Procedure_underwent.Code = Undergoes.Procedure_underwent AND Procedure_underwent.Name = '" + procedure_name + "';";
                run_query(con, query);
                break;
            default:
                System.out.println("Invalid choice. Please try again.");
                break;
            }
            System.out.println("Press any key to continue...\n\n");
            sc.nextLine();
        }
        sc.close();
        //Close the connection
        if(con != null) {
            try{
                con.close();
                System.out.println("Connection closed");
            } catch(SQLException e) {
                System.out.println("Error closing connection");
            }
        }
    }  
} 