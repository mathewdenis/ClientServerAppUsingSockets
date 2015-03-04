package clientserverapp;

import java.util.Scanner;

public class ClientServerApp 
{

    public static void main(String[] args) 
    {
        //Select server to send the file to
        Scanner input = new Scanner(System.in);

        System.out.println("Select the server to send the file:\n1. JAVA\n2. CPP");
        int choice = input.nextInt();
        Client.serverChoice = choice;
        
        //Start server thread
        Thread serverThread = new Thread(new Server());
        serverThread.start();
        
        //Start client thread
        Thread clientThread = new Thread(new Client());
        clientThread.start();
    }
    
}
