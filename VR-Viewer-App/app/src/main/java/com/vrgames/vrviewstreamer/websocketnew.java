package com.vrgames.vrviewstreamer;

import static com.google.vr.cardboard.ThreadUtils.runOnUiThread;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Base64;
import android.util.Log;
import org.java_websocket.client.WebSocketClient;
import org.java_websocket.handshake.ServerHandshake;
import org.java_websocket.client.WebSocketClient;
import org.java_websocket.handshake.ServerHandshake;
import java.net.URI;

public class websocketnew
{
    private WebSocketClient webSocketClient;
    private void connectWebSocket()
    {
        URI uri;
        try
        {
            uri = new URI("ws://45.55.49.156:3030");
        }
        catch (Exception e)
        {
            e.printStackTrace();
            return;
        }
        webSocketClient = new WebSocketClient(uri)
        {
            @Override
            public void onOpen(ServerHandshake serverHandshake)
            {
                Log.i("MSG", "Web socket bağlantısı başarılı bir şekilde açıldı!");
            }

            @Override
            public void onMessage(String message)
            {
                showImage(message);
            }

            @Override
            public void onClose(int i, String s, boolean b) {}

            @Override
            public void onError(Exception e)
            {
                e.printStackTrace();
            }
        };
        webSocketClient.connect();
    }
    private void showImage(final String base64Image)
    {
        byte[] decodedString = Base64.decode(base64Image, Base64.DEFAULT);
        final Bitmap decodedBitmap = BitmapFactory.decodeByteArray(decodedString, 0, decodedString.length);

        runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                if (decodedBitmap != null)
                {
                    //renderer.setBitmap(decodedBitmap);
                }
            }
        });
    }
}
