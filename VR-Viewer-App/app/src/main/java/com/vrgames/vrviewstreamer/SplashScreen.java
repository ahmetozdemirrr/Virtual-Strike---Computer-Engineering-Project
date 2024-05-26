package com.vrgames.vrviewstreamer;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import androidx.appcompat.app.AppCompatActivity;

import com.vrgames.vrviewstreamer.MainActivity;

public class SplashScreen extends AppCompatActivity
{
    private static final int SPLASH_SCREEN_DURATION = 1000; // Başlangıçta ilk ekranda 1.5 saniye duralım.

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.splash_screen);

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
        }, SPLASH_SCREEN_DURATION);
    }
}
