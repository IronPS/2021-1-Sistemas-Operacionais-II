import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import javax.websocket.*;
import javax.websocket.server.ServerEndpoint;

import java.util.concurrent.TimeoutException;
import java.util.logging.Logger;
import java.util.logging.Level;

@ServerEndpoint("/chat")
public class ChatServer{
	private final Logger LOGGER = Logger.getLogger(ChatServer.class.getName());
	private static Map<String, Session> webSessionMap = new HashMap<String, Session>();

	@OnOpen
	public void onOpen(final Session session) {
		String sessionID = session.getId();

		webSessionMap.remove(sessionID);

		webSessionMap.put(sessionID, session);
		
		LOGGER.log(Level.INFO, String.format("WebSocket Connect: %s", sessionID));
		
		try {
			session.getBasicRemote().sendText(String.format("[Welcome to Session %s!]", sessionID));
		} catch (IOException e) {
			LOGGER.log(Level.SEVERE, e.getMessage());
		}
	}
	
	@OnMessage
	public void onMessage(Session session, String msg) {
		try {
			for (Map.Entry<String, Session> entry : webSessionMap.entrySet()) {
				entry.getValue().getBasicRemote().sendText(String.format("[Session %s says:] %s", session.getId(), msg));
			}
		} catch (IOException e) {
			LOGGER.log(Level.SEVERE, e.getMessage());
		}
		
		LOGGER.log(Level.INFO, String.format("Message sent: %s", msg));
	}
	
	@OnClose
	public void onClose(final Session session) {
		String sessionID = session.getId();

		webSessionMap.remove(sessionID);
		
		LOGGER.log(Level.INFO, String.format("WebSocket Disconnect: %s", sessionID));
	}

	@OnError
	public void onError(Session session, Throwable t) {
		if (t instanceof TimeoutException) {
			LOGGER.log(Level.WARNING, String.format("WebSocket error on session %s: TIMEOUT", session.getId()));
			return;
		}
		LOGGER.log(Level.SEVERE, String.format("Websocket error on session %s: %s", session.getId(), t.getCause()));
	}

}
