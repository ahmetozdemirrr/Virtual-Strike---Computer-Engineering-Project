package com.vrgames.vrviewstreamer;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import androidx.appcompat.app.AppCompatActivity;

/**
 * SplashScreen class that displays a splash screen for a set duration
 * before transitioning to the MainActivity.
 */
public class SplashScreen extends AppCompatActivity
{
    /**
     * The duration for which the splash screen is displayed, in milliseconds.
     */
    private static final int SPLASH_SCREEN_DURATION = 1000; // 1 second

    /**
     * Called when the activity is first created.
     *
     * @param savedInstanceState If the activity is being re-initialized after
     *        previously being shut down then this Bundle contains the data it most
     *        recently supplied in onSaveInstanceState(Bundle). Note: Otherwise it is null.
     */
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.splash_screen);

        new Handler().postDelayed(new Runnable()
        {
            /**
             * This method will be executed once the timer is over.
             * It starts the MainActivity and finishes the SplashScreen activity.
             */
            @Override
            public void run()
            {
                // Start MainActivity after the splash screen duration
                Intent intent = new Intent(SplashScreen.this, MainActivity.class);
                startActivity(intent);
                // Close the SplashScreen activity
                finish();
            }
        }, SPLASH_SCREEN_DURATION);
    }
}
