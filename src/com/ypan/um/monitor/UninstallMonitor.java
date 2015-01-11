package com.ypan.um.monitor;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Scanner;

import android.content.Context;
import android.os.Build;
import android.util.Log;

public class UninstallMonitor {
	// 开始监控
	public static void startUninstallMonitor(Context ctx, String url) {
		String monitordir = ctx.getApplicationInfo().dataDir;
		String exe = "um";
		String exepath = ctx.getFilesDir() + File.separator + exe;
		boolean isSuccess = copyUmSuccess(ctx, exe, exepath);
		Log.e("test", "isSuccess = " + isSuccess);
		if (!isContainPs(exepath)) {
			Log.e("test", exepath + " is not exist!");
			runmonitor(exepath, exe, monitordir, url, Build.VERSION.SDK_INT);
		} else {
			Log.e("test", exepath + " is allready exist!");
		}

	}

	private static void chmod(String exepath) {
		try {
			String command = "chmod 755 " + exepath;
			Log.i("test", "command = " + command);
			Runtime runtime = Runtime.getRuntime();
			Process proc = runtime.exec(command);
		} catch (IOException e) {
			Log.i("test", "chmod fail!!!!");
			e.printStackTrace();
		}
	}

	private static void runmonitor(String exepath, String exe,
			String monitordir, String url, int version) {
		try {
			String command = exepath + " " + monitordir + " " + url + " "
					+ version;
			Log.i("test", "command = " + command);
			Runtime runtime = Runtime.getRuntime();
			Process proc = runtime.exec(command);
		} catch (IOException e) {
			Log.i("test", "chmod fail!!!!");
			e.printStackTrace();
		}
	}

	private static boolean copyUmSuccess(Context ctx, String name,
			String exepath) {
		File file = new File(exepath);
		if (file.exists()) {
			return true;
		}
		try {
			copyAssertFile(ctx, name, file);
			chmod(exepath);
		} catch (IOException e) {
			e.printStackTrace();
			Log.i("test", "copy fail!!!!");
		}
		return file.exists();
	}

	// 复制文件
	private static void copyAssertFile(Context ctx, String name, File targetFile)
			throws IOException {
		BufferedInputStream inBuff = null;
		BufferedOutputStream outBuff = null;
		try {
			// 新建文件输入流并对它进行缓冲
			inBuff = new BufferedInputStream(ctx.getAssets().open(name));

			// 新建文件输出流并对它进行缓冲
			outBuff = new BufferedOutputStream(new FileOutputStream(targetFile));

			// 缓冲数组
			byte[] b = new byte[1024 * 5];
			int len;
			while ((len = inBuff.read(b)) != -1) {
				outBuff.write(b, 0, len);
			}
			// 刷新此缓冲的输出流
			outBuff.flush();
		} finally {
			// 关闭流
			if (inBuff != null)
				inBuff.close();
			if (outBuff != null)
				outBuff.close();
		}
	}

	// 复制文件
	private static void copyFile(File sourceFile, File targetFile)
			throws IOException {
		BufferedInputStream inBuff = null;
		BufferedOutputStream outBuff = null;
		try {
			// 新建文件输入流并对它进行缓冲
			inBuff = new BufferedInputStream(new FileInputStream(sourceFile));

			// 新建文件输出流并对它进行缓冲
			outBuff = new BufferedOutputStream(new FileOutputStream(targetFile));

			// 缓冲数组
			byte[] b = new byte[1024 * 5];
			int len;
			while ((len = inBuff.read(b)) != -1) {
				outBuff.write(b, 0, len);
			}
			// 刷新此缓冲的输出流
			outBuff.flush();
		} finally {
			// 关闭流
			if (inBuff != null)
				inBuff.close();
			if (outBuff != null)
				outBuff.close();
		}
	}

	// 读取输入流，将它转为字符串
	private static ArrayList<String> readFully(InputStream inputStream)
			throws IOException {
		ArrayList<String> list = new ArrayList<String>();
		Scanner lScanner = new Scanner(inputStream);
		try {
			while (lScanner.hasNextLine()) {
				list.add(lScanner.nextLine());
			}
		} catch (Exception e) {

		} finally {
			lScanner.close();
		}

		return list;
	}

	private static ArrayList<String> readAllProcess() {
		try {
			return readFully(new ProcessBuilder(new String[] { "ps" }).start()
					.getInputStream());
		} catch (IOException e) {

		}
		return new ArrayList<String>();
	}

	private static boolean isContainPs(String exepath) {
		boolean isContain = false;
		ArrayList<String> allps = readAllProcess();
		for (String ps : allps) {
			if (ps.contains(exepath)) {
				isContain = true;
				break;
			}
		}
		return isContain;
	}
}
