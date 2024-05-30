package com.vrgames.vrviewstreamer;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import androidx.appcompat.app.AppCompatActivity;

import com.vrgames.vrviewstreamer.MainActivity;

/**
 * SplashScreen activity that shows a splash screen for a certain duration
 * before transitioning to the main activity.
 */
public class SplashScreen extends AppCompatActivity 
{
    // Duration the splash screen will be shown (in milliseconds)
    private static final int SPLASH_SCREEN_DURATION = 1500; // Başlangıçta ilk ekranda 1.5 saniye duralım.

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
        setContentView(R.layout.splash_screen);

        // Handler to delay the transition from the splash screen to the main activity
        new Handler().postDelayed(new Runnable() 
        {
            @Override
            public void run() 
            {
                // Splash ekranı gösterildikten sonra ana aktiviteye geçiş yap
                Intent intent = new Intent(SplashScreen.this, MainActivity.class);
                startActivity(intent);
                finish(); // Splash aktivitesini kapat
            }
        }, 
        SPLASH_SCREEN_DURATION);
    }
}
