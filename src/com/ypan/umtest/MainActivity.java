package com.ypan.umtest;

import android.app.Activity;
import android.os.Bundle;

import com.ypan.um.monitor.UninstallMonitor;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		UninstallMonitor.start(this, "http://www.haosou.com/");
	}
}
