package com.vrgames.vrviewstreamer;

import android.opengl.GLES20;
import android.opengl.Matrix;
import android.os.Bundle;
import android.util.Base64;
import android.util.Log;
import com.google.vr.sdk.base.Eye;
import com.google.vr.sdk.base.GvrActivity;
import com.google.vr.sdk.base.GvrView;
import com.google.vr.sdk.base.HeadTransform;
import com.google.vr.sdk.base.Viewport;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import org.java_websocket.client.WebSocketClient;
import org.java_websocket.handshake.ServerHandshake;
import java.net.URI;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import javax.microedition.khronos.egl.EGLConfig;
public class MainActivity extends GvrActivity implements GvrView.StereoRenderer
{
    private static final float Z_NEAR = 0.1f;
    private static final float Z_FAR = 100.0f;
    private WebSocketClient webSocketClient;
    private WebSocketClient sensorSocket;
    private boolean isRunning = true;
    private float[] mView;
    private float[] mCamera;
    private int mTextureDataHandle;
    private FloatBuffer mQuadVertices;
    private FloatBuffer mQuadTexCoord;
    private int mQuadProgram;
    private int mQuadPositionParam;
    private int mQuadTexCoordParam;
    private int mModelViewProjectionParam;
    private static final int COORDS_PER_VERTEX = 3;
    private static final int TEXCOORDS_PER_VERTEX = 2;
    private volatile Bitmap bitmap;
    String ivmeolcer;
    boolean appisOpen = true;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        connectWebSocket();
        connectsensorSocket();
        GvrView gvrView = findViewById(R.id.gvr_view);
        gvrView.setRenderer(this);
        setGvrView(gvrView);

        mView = new float[16];
        mCamera = new float[16];

        // Quad vertices
        final float[] QUAD_COORDS = new float[]
        {
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,
        };
        mQuadVertices = ByteBuffer.allocateDirect(QUAD_COORDS.length * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mQuadVertices.put(QUAD_COORDS).position(0);

        // Quad texture coordinates
        final float[] QUAD_TEX_COORDS = new float[]
        {
                0.0f, 1.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f,
        };
        mQuadTexCoord = ByteBuffer.allocateDirect(QUAD_TEX_COORDS.length * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mQuadTexCoord.put(QUAD_TEX_COORDS).position(0);
    }
    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        // Uygulama kapandığında Websocket bağlantısı kapatılır
        if (webSocketClient != null)
        {
            webSocketClient.close();
        }
        isRunning = false; // Döngüyü durdur
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        // Uygulama durduğunda sensör dinlemeyi kapat
    }

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
                Log.i("MSGOPEN", "Web socket bağlantısı başarılı bir şekilde açıldı!");
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
        new Thread(new Runnable()
        {
            @Override
            public void run()
            {
                byte[] decodedString = Base64.decode(base64Image, Base64.DEFAULT);
                bitmap = BitmapFactory.decodeByteArray(decodedString, 0, decodedString.length);
            }
        }).start();
    }

    @Override
    public void onSurfaceCreated(EGLConfig eglConfig)
    {
        mQuadProgram = createQuadProgram();
        mQuadPositionParam = GLES20.glGetAttribLocation(mQuadProgram, "a_Position");
        mQuadTexCoordParam = GLES20.glGetAttribLocation(mQuadProgram, "a_TexCoord");
        mModelViewProjectionParam = GLES20.glGetUniformLocation(mQuadProgram, "u_MVP");
        updateSurface();
    }
    public void updateSurface()
    {
        //GLES20.glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        GLES20.glEnable(GLES20.GL_DEPTH_TEST);

        if(appisOpen==true)
        {
            Log.i("appisOpen",String.valueOf(appisOpen));
            mTextureDataHandle = loadTexture(this, R.drawable.splash);
            appisOpen=false;
        }
        if(bitmap!=null)
        {
            mTextureDataHandle = newloadTexture();
        }
    }

    @Override
    public void onDrawEye(Eye eye)
    {
        updateSurface();
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
        float[] perspective = eye.getPerspective(Z_NEAR, Z_FAR);
        drawQuad(perspective);
    }

    private void drawQuad(float[] perspective)
    {
        Log.i("drawQuad","oooooooo");

        GLES20.glUseProgram(mQuadProgram);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureDataHandle);

        mQuadVertices.position(0);
        GLES20.glVertexAttribPointer(mQuadPositionParam, COORDS_PER_VERTEX, GLES20.GL_FLOAT,false, 0, mQuadVertices);
        GLES20.glEnableVertexAttribArray(mQuadPositionParam);

        mQuadTexCoord.position(0);
        GLES20.glVertexAttribPointer(mQuadTexCoordParam, TEXCOORDS_PER_VERTEX, GLES20.GL_FLOAT, false, 0, mQuadTexCoord);
        GLES20.glEnableVertexAttribArray(mQuadTexCoordParam);

        float[] modelMatrix = new float[16];

        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.translateM(modelMatrix, 0, -0.0f, 0.0f, -0.9f);
        Matrix.scaleM(modelMatrix, 0, 1.0f, 1.0f , 1.0f);

        float[] mvpMatrix = new float[16];

        Matrix.multiplyMM(mvpMatrix, 0, perspective, 0, modelMatrix, 0);
        GLES20.glUniformMatrix4fv(mModelViewProjectionParam, 1, false, mvpMatrix, 0);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }
    public void connectsensorSocket()
    {
        URI uri;
        try
        {
            uri = new URI("ws://45.55.49.156:3000");
        }

        catch (Exception e)
        {
            e.printStackTrace();
            return;
        }
        sensorSocket = new WebSocketClient(uri)
        {
            @Override
            public void onOpen(ServerHandshake serverHandshake)
            {
                System.out.println("WebSocket connection opened");
                // Bağlantı açıldığında yapılacak işlemler
            }

            @Override
            public void onMessage(String s)
            {
                System.out.println("Received message: " + s);
                // Gelen mesajları işle
            }

            @Override
            public void onClose(int i, String s, boolean b)
            {
                System.out.println("WebSocket connection closed");
                // Bağlantı kapandığında yapılacak işlemler
            }

            @Override
            public void onError(Exception e)
            {
                e.printStackTrace();
                // Hata durumunda yapılacak işlemler
            }
        };
        sensorSocket.connect();
    }
    @Override
    public void onNewFrame(HeadTransform headTransform)
    {
        headTransform.getHeadView(mCamera, 0);
        ivmeolcer = String.valueOf(mCamera[0]) + " " + String.valueOf(mCamera[1]) + " " + String.valueOf(mCamera[2]);

        if (sensorSocket != null && sensorSocket.isOpen())
        {
            Log.i("ACC",ivmeolcer );
            sensorSocket.send(ivmeolcer);
        }

        else
        {
            Log.e("TAG10", "Web socket bağlantısı henüz açılmadı veya kapatıldı.");
        }
    }

    @Override
    public void onSurfaceChanged(int width, int height)
    {
        GLES20.glViewport(0, 0, width, height);
    }

    @Override
    public void onFinishFrame(Viewport viewport) {}

    @Override
    public void onRendererShutdown() {}

    // Eğer kaynaklara ihtiyacınız varsa (örneğin, bir VR deneyimindeki ses dosyaları), buraya ekleyebilirsiniz.
    private int newloadTexture()
    {
        final int[] textureHandle = new int[1];

        GLES20.glGenTextures(1, textureHandle, 0);

        if (textureHandle[0] != 0)
        {
            // Bind to the texture in OpenGL
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);

            // Load the bitmap into the bound texture
            android.opengl.GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);

            // Recycle the bitmap, since its data has been loaded into OpenGL
            bitmap.recycle();
        }

        if (textureHandle[0] == 0)
        {
            throw new RuntimeException("Error loading texture.");
        }

        return textureHandle[0];
    }
    private int loadTexture(Context context, int resourceId)
    {
        final int[] textureHandle = new int[1];
        GLES20.glGenTextures(1, textureHandle, 0);

        if (textureHandle[0] != 0)
        {
            final BitmapFactory.Options options = new BitmapFactory.Options();
            options.inScaled = false;   // No pre-scaling

            // Load the bitmap
            final Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), resourceId, options);
            // Bind to the texture in OpenGL
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);

            // Set filtering
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
            // Load the bitmap into the bound texture
            android.opengl.GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
            // Recycle the bitmap, since its data has been loaded into OpenGL
            bitmap.recycle();
        }

        if (textureHandle[0] == 0)
        {
            throw new RuntimeException("Error loading texture.");
        }
        return textureHandle[0];
    }

    private int createQuadProgram()
    {
        final String vertexShaderCode =
                "attribute vec4 a_Position;\n" +
                "attribute vec2 a_TexCoord;\n" +
                "varying vec2 v_TexCoord;\n" +
                "uniform mat4 u_MVP;\n" +
                "void main() {\n" +
                "  v_TexCoord = a_TexCoord;\n" +
                "  gl_Position = u_MVP * a_Position;\n" +
                "}\n";

        final String fragmentShaderCode =
                "precision mediump float;\n" +
                "varying vec2 v_TexCoord;\n" +
                "uniform sampler2D u_Texture;\n" +
                "void main() {\n" +
                "  gl_FragColor = texture2D(u_Texture, v_TexCoord);\n" +
                "}\n";

        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode);
        int fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode);
        int program = GLES20.glCreateProgram();

        GLES20.glAttachShader(program, vertexShader);
        GLES20.glAttachShader(program, fragmentShader);
        GLES20.glLinkProgram(program);

        return program;
    }

    private int loadShader(int type, String shaderCode)
    {
        int shader = GLES20.glCreateShader(type);
        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);
        return shader;
    }
}