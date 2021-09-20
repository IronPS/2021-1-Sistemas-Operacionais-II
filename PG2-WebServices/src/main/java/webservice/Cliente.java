package webservice;

import javax.ws.rs.core.MediaType;

import com.sun.jersey.api.client.Client;
import com.sun.jersey.api.client.ClientResponse;
import com.sun.jersey.api.client.WebResource;
import com.sun.jersey.api.client.config.ClientConfig;
import com.sun.jersey.api.client.config.DefaultClientConfig;

public class Cliente {
     
    public static void main(String[] args) {
    	ClientConfig config = new DefaultClientConfig(); 
        Client cliente = Client.create(config); 
        WebResource servico = cliente.resource("http://localhost:9000/calculadora");
        
        if (args.length != 3) {
            System.out.println(String.format("%d", args.length));

            System.out.println("Usage: operation integer integer\nOperands: + *\nExample: + 3 2");
        }
        
        int a = 0;
        int b = 0;
        try {
        	a = Integer.parseInt(args[1]);
        	b = Integer.parseInt(args[2]);
        } catch (Exception e) {
        	e.printStackTrace();
        	System.exit(0);
        }

        if (args[0].equals("+")) {
        	WebResource serv = servico.path("somarInt").path(a + "/" + b);
        	getResponse(serv);

        } else if (args[0].equals("*")) {
        	WebResource serv = servico.path("multiplicarInt").path(a + "/" + b);
        	getResponse(serv);
        	
        } else {
        	System.out.println("Valid operands are * and +");
            System.exit(0);
        }
        
        
    }
    
    private static void getResponse(WebResource serv) {
    	ClientResponse respostaServ =         		   
        		serv.accept(MediaType.APPLICATION_JSON).get(ClientResponse.class);
        
        String respostaServicoStr = respostaServ.getEntity(String.class);
        System.out.println("Resposta da operacao: " + respostaServicoStr);
    }
    
}
