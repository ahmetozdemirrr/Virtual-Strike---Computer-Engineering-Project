package com.vrgames.vrviewstreamer;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
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
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import javax.microedition.khronos.egl.EGLConfig;

/**
 * MainActivity class that sets up the VR environment, handles sensor data, and communicates with a server.
 */
public class MainActivity extends GvrActivity implements GvrView.StereoRenderer 
{
    private static final float Z_NEAR = 0.1f;
    private static final float Z_FAR = 100.0f;
    private static final int COORDS_PER_VERTEX = 3;
    private static final int TEXCOORDS_PER_VERTEX = 2;
    //private static String IPSocket = "";
    private static String IPSocket = "";
    private final KalmanFilter kalmanFilter = new KalmanFilter(16);
    private final Object leftTextureLock = new Object();
    private final Object rightTextureLock = new Object();
    private final ExecutorService executorService = Executors.newFixedThreadPool(2);
    private volatile Bitmap bitmapLeft;
    private volatile Bitmap bitmapRight;
    private WebSocketClient webSocketClient;
    private WebSocketClient sensorSocket;
    private int mQuadProgram;
    private int mQuadPositionParam;
    private int mQuadTexCoordParam;
    private int mModelViewProjectionParam;    
    private int mTextureDataHandleLeft;
    private int mTextureDataHandleRight;
    private SensorManager sensorManager;
    private Sensor accelerometer;   
    private FloatBuffer mQuadVertices;
    private FloatBuffer mQuadTexCoord;
    private float[] mCamera;
    private boolean isLeftEye = true;
    String ivmeolcer;
    boolean appisOpen = true;

    //private static String IPSocket = "192.168.148.137";
    
    /**
     * Called when the activity is first created.
     *
     * @param savedInstanceState If the activity is being re-initialized after
     *                           previously being shut down then this Bundle contains the data it most
     *                           recently supplied in onSaveInstanceState(Bundle). <b>Note: Otherwise it is null.</b>
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initializeQuadData();
        connectWebSocket();
        connectSensorSocket();
        GvrView gvrView = findViewById(R.id.gvr_view);
        gvrView.setRenderer(this);
        setGvrView(gvrView);

        mCamera = new float[16];
        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

        // Initialize quad vertices and texture coordinates
    }

    /**
     * Called when the activity is paused.
     */
    @Override
    protected void onPause() 
    {
        super.onPause();
        webSocketClient.close();
        sensorSocket.close();
        GLES20.glDeleteTextures(1, new int[]{mTextureDataHandleLeft}, 0);
        GLES20.glDeleteTextures(1, new int[]{mTextureDataHandleRight}, 0);
        GLES20.glDeleteProgram(mQuadProgram);
    }

    /**
     * Initialize the quad data for rendering.
     */
    private void initializeQuadData() 
    {
        final float[] QUAD_COORDS = new float[]
        {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
        };
        mQuadVertices = ByteBuffer.allocateDirect(QUAD_COORDS.length * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mQuadVertices.put(QUAD_COORDS).position(0);

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

    /**
     * Update the texture with new data.
     *
     * @param byteArray The byte array containing the texture data.
     */
    private synchronized void updateTexture(byte[] byteArray) 
    {
        Bitmap bmp = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.length);

        if (bmp != null) 
        {
            Log.i("WebSocket", "Bitmap created with dimensions: " + bmp.getWidth() + "x" + bmp.getHeight());
        } 

        else 
        {
            Log.e("WebSocket", "Failed to decode byte array to bitmap");
        }

        if (isLeftEye) 
        {
            bitmapLeft = bmp;
        } 

        else 
        {
            bitmapRight = bmp;
        }
        isLeftEye = !isLeftEye;
    }

    /**
     * Draw the scene for a given eye.
     *
     * @param eye The eye being rendered.
     */
    @Override
    public void onDrawEye(Eye eye) 
    {
        updateSurface();
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);

        float[] perspective = eye.getPerspective(Z_NEAR, Z_FAR);
        int textureHandle;

        synchronized (this) 
        {
            textureHandle = (eye.getType() == Eye.Type.LEFT) ? mTextureDataHandleLeft : mTextureDataHandleRight;
        }
        drawQuad(perspective, textureHandle, eye);
    }

    /**
     * Update the surface textures.
     */
    private void updateSurface() 
    {
        GLES20.glEnable(GLES20.GL_DEPTH_TEST);

        synchronized (this) 
        {
            if (appisOpen) 
            {
                mTextureDataHandleLeft = loadTexture(this, R.drawable.splash);
                mTextureDataHandleRight = loadTexture(this, R.drawable.splash);
                appisOpen = false;
            }

            if (bitmapLeft != null) 
            {
                GLES20.glDeleteTextures(1, new int[]{mTextureDataHandleLeft}, 0);
                mTextureDataHandleLeft = loadTexture(bitmapLeft);
                bitmapLeft.recycle();
                bitmapLeft = null;
            }

            if (bitmapRight != null) 
            {
                GLES20.glDeleteTextures(1, new int[]{mTextureDataHandleRight}, 0);
                mTextureDataHandleRight = loadTexture(bitmapRight);
                bitmapRight.recycle();
                bitmapRight = null;
            }
        }
    }
	
	/**
     * Connect to the WebSocket server.
     */
     private void connectWebSocket() 
     {
        URI uri;

        try 
        {
            uri = new URI("ws://"+IPSocket+":3030");
        } 
        
        catch (Exception e) 
        {
            Log.e("WebSocket", "URI error", e);
            return;
        }

        webSocketClient = new WebSocketClient(uri) 
        {
            @Override
            public void onOpen(ServerHandshake serverHandshake) 
            {
                Log.i("WebSocket", "Connection opened");
            }

            @Override
            public void onMessage(String message) 
            {
                Log.i("WebSocket", "Message received: " + message);
            }

            @Override
            public void onMessage(ByteBuffer bytes) 
            {
                byte[] byteArray = new byte[bytes.remaining()];
                bytes.get(byteArray);
                updateTexture(byteArray);
            }

            @Override
            public void onClose(int code, String reason, boolean remote) 
            {
                Log.i("WebSocket", "Closed " + reason);
            }

            @Override
            public void onError(Exception e) 
            {
                Log.e("WebSocket", "Error ", e);
            }
        };
        webSocketClient.connect();
    }

    /**
     * Connect to the sensor WebSocket server.
     */
    private void connectSensorSocket()
    {
        URI uri;

        try
        {
            uri = new URI("ws://"+IPSocket+":3000");
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
                Log.i("SensorSocket", "Connection opened sensor");
            }

            @Override
            public void onMessage(String s)
            {

            }

            @Override
            public void onClose(int i, String s, boolean b)
            {
                Log.i("SensorSocket", "Connection closed");
            }

            @Override
            public void onError(Exception e)
            {
                e.printStackTrace();
            }
        };
        sensorSocket.connect();
    }
    
    /**
     * Called when a new frame is ready.
     *
     * @param headTransform The head transform data.
     */
    @Override
    public void onNewFrame(HeadTransform headTransform) 
    {
        float[] currentHeadView = new float[16];
        headTransform.getHeadView(currentHeadView, 0);

        // Apply Kalman filter
        float[] mFilteredHeadView = kalmanFilter.update(currentHeadView);
        // Update the camera view with the filtered head view
        System.arraycopy(mFilteredHeadView, 0, mCamera, 0, 16);

        if (accelerometer != null) 
        {
            // Önce bir dinleyici ekleyin
            SensorEventListener sensorEventListener = new SensorEventListener() 
            {
                @Override
                public void onSensorChanged(SensorEvent event) 
                {
                    float zValue = event.values[2];
                    float xValue = event.values[0];
                    Log.d("deneme222", "Z Value: " + zValue);
                    // Sensör verilerini aldıktan sonra dinleyiciyi kaldırabilirsiniz
                    sensorManager.unregisterListener(this);
                    // Send the filtered head view data to the server
                    ivmeolcer = String.valueOf(mCamera[0]) + " " + zValue + " " + String.valueOf(mCamera[2]) + " " + xValue;

                    if (sensorSocket != null && sensorSocket.isOpen()) 
                    {
                        sensorSocket.send(ivmeolcer);
                    }
                }

                @Override
                public void onAccuracyChanged(Sensor sensor, int accuracy) 
                {
                    // Sensör doğruluğu değişikliklerini burada işleyebilirsiniz.
                }
            };
            sensorManager.registerListener(sensorEventListener, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
        }

        // Reset the sensor values to zero after sending
        Matrix.setIdentityM(currentHeadView, 0);
        headTransform.getHeadView(currentHeadView, 0);
    }

    /**
     * Draw a quad on the screen.
     *
     * @param perspective The perspective matrix.
     * @param textureHandle The texture handle to use.
     * @param eye The eye being rendered.
     */
    private void drawQuad(float[] perspective, int textureHandle, Eye eye) 
    {
        GLES20.glUseProgram(mQuadProgram);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle);

        mQuadVertices.position(0);
        GLES20.glVertexAttribPointer(mQuadPositionParam, COORDS_PER_VERTEX, GLES20.GL_FLOAT, false, 0, mQuadVertices);
        GLES20.glEnableVertexAttribArray(mQuadPositionParam);

        mQuadTexCoord.position(0);
        GLES20.glVertexAttribPointer(mQuadTexCoordParam, TEXCOORDS_PER_VERTEX, GLES20.GL_FLOAT, false, 0, mQuadTexCoord);
        GLES20.glEnableVertexAttribArray(mQuadTexCoordParam);

        float[] modelMatrix = new float[16];
        Matrix.setIdentityM(modelMatrix, 0);

        if ((eye.getType() == Eye.Type.LEFT)) 
        {
            Matrix.translateM(modelMatrix, 0, 0.15f, 0.0f, -0.9f);
        } 

        else 
        {
            Matrix.translateM(modelMatrix, 0, -0.15f, 0.0f, -0.9f);
        }

        Matrix.scaleM(modelMatrix, 0, 1.0f, 1.0f, 1.0f);

        float[] mvpMatrix = new float[16];
        Matrix.multiplyMM(mvpMatrix, 0, perspective, 0, modelMatrix, 0);
        GLES20.glUniformMatrix4fv(mModelViewProjectionParam, 1, false, mvpMatrix, 0);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    /**
     * Called when the renderer is shut down.
     */
    @Override
    public void onRendererShutdown() 
    {
        Log.i("MainActivity", "Selam ben Ahmet");
    }

    @Override
    public void onFinishFrame(Viewport viewport) {}

    /**
     * Called when the surface size changes.
     *
     * @param width The new width.
     * @param height The new height.
     */
    @Override
    public void onSurfaceChanged(int width, int height) 
    {
        GLES20.glViewport(0, 0, width, height);
    }

    /**
     * Called when the surface is created.
     *
     * @param eglConfig The EGL configuration.
     */
    @Override
    public void onSurfaceCreated(EGLConfig eglConfig) 
    {
        mQuadProgram = createQuadProgram();
        mQuadPositionParam = GLES20.glGetAttribLocation(mQuadProgram, "a_Position");
        mQuadTexCoordParam = GLES20.glGetAttribLocation(mQuadProgram, "a_TexCoord");
        mModelViewProjectionParam = GLES20.glGetUniformLocation(mQuadProgram, "u_MVP");
        updateSurface();
    }

    /**
     * Create a program for rendering a quad.
     *
     * @return The created program handle.
     */
    private int createQuadProgram() {
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
            "  vec4 textureColor = texture2D(u_Texture, v_TexCoord);\n" +
            "  float distance = distance(v_TexCoord, vec2(0.5, 0.5));\n" +
            "  if (distance < 0.005) {\n" + // Nişangah boyutunu ayarlamak için bu değeri küçültelim dogukan!!!
            "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n" + // Kırmızı renk
            "  } else {\n" +
            "    gl_FragColor = textureColor;\n" +
            "  }\n" +
            "}\n";

        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode);
        int fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode);
        int program = GLES20.glCreateProgram();

        GLES20.glAttachShader(program, vertexShader);
        GLES20.glAttachShader(program, fragmentShader);
        GLES20.glLinkProgram(program);

        return program;
    }

    /**
     * Load a shader.
     *
     * @param type The type of shader to load.
     * @param shaderCode The shader code.
     * @return The created shader handle.
     */
    private int loadShader(int type, String shaderCode) 
    {
        int shader = GLES20.glCreateShader(type);
        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);

        return shader;
    }

    /**
     * Load a texture from a bitmap.
     *
     * @param bitmap The bitmap to load the texture from.
     * @return The created texture handle.
     */
    private int loadTexture(Bitmap bitmap) 
    {
        final int[] textureHandle = new int[1];
        GLES20.glGenTextures(1, textureHandle, 0);

        if (textureHandle[0] != 0) 
        {
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);
            android.opengl.GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
        }

        if (textureHandle[0] == 0) 
        {
            throw new RuntimeException("Error loading texture.");
        }
        return textureHandle[0];
    }

    /**
     * Load a texture from a resource ID.
     *
     * @param context The context to use.
     * @param resourceId The resource ID of the texture.
     * @return The created texture handle.
     */
    private int loadTexture(Context context, int resourceId) 
    {
        final int[] textureHandle = new int[1];
        GLES20.glGenTextures(1, textureHandle, 0);

        if (textureHandle[0] != 0) 
        {
            final BitmapFactory.Options options = new BitmapFactory.Options();
            options.inScaled = false;
            final Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), resourceId, options);
            
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
            android.opengl.GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
            bitmap.recycle();
        }

        if (textureHandle[0] == 0) 
        {
            throw new RuntimeException("Error loading texture.");
        }
        return textureHandle[0];
    }

    /**
     * Kalman filter implementation to reduce sensor noise.
     */
    static class KalmanFilter 
    {
        private final float[] stateEstimate;
        private final float[] errorCovariance;
        private final float[] processNoise;
        private final float[] measurementNoise;

        /**
         * Constructor for the KalmanFilter.
         *
         * @param size The size of the state vector.
         */
        public KalmanFilter(int size) 
        {
            stateEstimate = new float[size];
            errorCovariance = new float[size];
            processNoise = new float[size];
            measurementNoise = new float[size];

            for (int i = 0; i < size; i++) 
            {
                errorCovariance[i] = 1.0f; // Initialize with some reasonable value
                processNoise[i] = 1.0f; // Adjust this value based on your process
                measurementNoise[i] = 1.0f; // Adjust this value based on your measurements
            }
        }

        /**
         * Update the state estimate with a new measurement.
         *
         * @param measurement The new measurement.
         * @return The updated state estimate.
         */
        public float[] update(float[] measurement) 
        {
            float[] newEstimate = new float[stateEstimate.length];

            for (int i = 0; i < stateEstimate.length; i++) 
            {
                // Kalman gain
                float kalmanGain = errorCovariance[i] / (errorCovariance[i] + measurementNoise[i]);

                // Update estimate with measurement
                newEstimate[i] = stateEstimate[i] + kalmanGain * (measurement[i] - stateEstimate[i]);
                // Update error covariance
                errorCovariance[i] = (1 - kalmanGain) * errorCovariance[i] + processNoise[i];
                stateEstimate[i] = newEstimate[i];
            }
            return newEstimate;
        }
    }
}
