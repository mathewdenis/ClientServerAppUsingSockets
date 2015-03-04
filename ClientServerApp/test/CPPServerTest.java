import clientserverapp.Client;
import clientserverapp.Server;
import static java.lang.Thread.sleep;
import org.junit.Test;


public class CPPServerTest {
    
    @Test
    public void CPPServerTest() 
    {
        //Set the server to CPP
        Client.serverChoice = 2;
        
        //Start server thread
        Thread serverThread = new Thread(new Server());
        serverThread.start();
        
        //Start client thread
        Thread clientThread = new Thread(new Client());
        clientThread.start();
        
        try
        {
            sleep(5000);
        }
        catch(Exception e)
        {
            assert(false);
        }
        
        assert(!Client.errorState);
    }
}
