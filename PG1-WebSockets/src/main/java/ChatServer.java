import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import javax.websocket.OnClose;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;

import java.util.logging.Logger;
import java.util.logging.Level;

@ServerEndpoint("/chat")
public class ChatServer{
	private final Logger LOGGER = Logger.getLogger(ChatServer.class.getName());
	private static Map<String, Session> webSessionMap = new HashMap<String, Session>();

	@OnOpen
	public void onOpen(final Session session) {
		String sessionID = session.getId();
			
		if (webSessionMap.containsKey(sessionID)) {
			webSessionMap.remove(sessionID);
		} 

		webSessionMap.put(sessionID, session);
		
		LOGGER.log(Level.INFO, String.format("WebSocket Connect: %s", sessionID));
		
		try {
			session.getBasicRemote().sendText(String.format("Welcome to Session %s!", sessionID));
		} catch (IOException e) {
			LOGGER.log(Level.SEVERE, e.getMessage());
		}
	}
	
	@OnMessage
	public void onMessage(Session session, String msg) {
		try {
			for (Map.Entry<String, Session> entry : webSessionMap.entrySet()) {
				entry.getValue().getBasicRemote().sendText(msg);

			}
		} catch (IOException e) {
			LOGGER.log(Level.SEVERE, e.getMessage());
		}
		
		LOGGER.log(Level.INFO, String.format("Message sent: %s", msg));

	}
	
	@OnClose
	public void onClose(final Session session) {
		String sessionID = session.getId();
		
		if (webSessionMap.containsKey(sessionID)) {
			webSessionMap.remove(sessionID);
				
		}
		
		LOGGER.log(Level.INFO, String.format("WebSocket Disconnect: %s", sessionID));
	}

}
