/*
 * Copyright (C) 2026 Zhou Qiankang <wszqkzqk@qq.com>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * This file is part of PvZ-Portable.
 *
 * PvZ-Portable is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PvZ-Portable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with PvZ-Portable. If not, see <https://www.gnu.org/licenses/>.
 */

package io.github.wszqkzqk.pvzportable;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class ResourceImportActivity extends AppCompatActivity {
    private static final String TAG = "ResImport";
    private static final int BUFFER_SIZE = 8192;

    private File gameDir;
    private TextView statusText;
    private ProgressBar progressBar;
    private EditText urlInput;
    private Button btnDownloadImport;
    private Button btnLaunchGame;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_resource_import);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(android.R.id.content), (v, insets) -> {
            Insets bars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(bars.left, bars.top, bars.right, bars.bottom);
            return insets;
        });

        gameDir = getExternalFilesDir(null);
        if (gameDir != null && !gameDir.exists()) gameDir.mkdirs();

        statusText = findViewById(R.id.status_text);
        progressBar = findViewById(R.id.progress_bar);
        urlInput = findViewById(R.id.url_input);
        String default_url = "";
        if (urlInput != null && urlInput.getText().toString().isEmpty() && default_url != "") {
            urlInput.setText(default_url);
        }
        btnDownloadImport = findViewById(R.id.btn_download_import);
        btnLaunchGame = findViewById(R.id.btn_launch_game);

        btnDownloadImport.setOnClickListener(v ->
            downloadAndImport(urlInput.getText().toString().trim())
        );
        btnLaunchGame.setOnClickListener(v -> launchGame());

        refreshStatus();
    }

    private boolean hasResources() {
        if (gameDir == null) return false;
        File pak = new File(gameDir, "main.pak");
        File props = new File(gameDir, "properties");
        return pak.exists() && props.isDirectory();
    }

    private void refreshStatus() {
        boolean ready = hasResources();
        if (ready) {
            statusText.setText(R.string.status_ready);
            btnLaunchGame.setEnabled(true);
        } else {
            statusText.setText(R.string.status_missing);
            btnLaunchGame.setEnabled(false);
        }
        progressBar.setVisibility(View.GONE);
    }

    private void launchGame() {
        Intent intent = new Intent(this, PvZPortableActivity.class);
        startActivity(intent);
        finish();
    }

    private void downloadAndImport(String urlString) {
        if (urlString.isEmpty()) {
            Toast.makeText(this, R.string.enter_url, Toast.LENGTH_SHORT).show();
            return;
        }

        setWorking(true);
        new Thread(() -> {
            File tempZip = null;
            HttpURLConnection conn = null;
            try {
                URL url = new URL(urlString);
                conn = (HttpURLConnection) url.openConnection();
                conn.setConnectTimeout(15000);
                conn.setReadTimeout(30000);
                conn.connect();

                int responseCode = conn.getResponseCode();
                if (responseCode != HttpURLConnection.HTTP_OK) {
                    throw new IOException("HTTP " + responseCode + ": " + conn.getResponseMessage());
                }

                tempZip = File.createTempFile("import_", ".zip", getCacheDir());
                try (InputStream is = conn.getInputStream();
                     OutputStream os = new BufferedOutputStream(new FileOutputStream(tempZip), BUFFER_SIZE)) {
                    byte[] buf = new byte[BUFFER_SIZE];
                    int len;
                    while ((len = is.read(buf)) > 0) os.write(buf, 0, len);
                }
                conn.disconnect();
                conn = null;

                extractZip(tempZip);

            } catch (Exception e) {
                Log.e(TAG, "Download failed", e);
                runOnUiThread(() -> {
                    Toast.makeText(this, getString(R.string.import_failed, e.getMessage()), Toast.LENGTH_LONG).show();
                    refreshStatus();
                });
            } finally {
                if (conn != null) conn.disconnect();
                if (tempZip != null && tempZip.exists()) tempZip.delete();
                runOnUiThread(() -> setWorking(false));
            }
        }).start();
    }

    private void extractZip(File zipFile) {
        try (InputStream is = new FileInputStream(zipFile);
             ZipInputStream zis = new ZipInputStream(is)) {
            ZipEntry entry;
            while ((entry = zis.getNextEntry()) != null) {
                if (entry.isDirectory()) {
                    zis.closeEntry();
                    continue;
                }
                String name = stripCommonPrefix(entry.getName());
                if (name == null) { zis.closeEntry(); continue; }

                File outFile = new File(gameDir, name);
                File parent = outFile.getParentFile();
                if (parent != null && !parent.exists()) parent.mkdirs();

                try (OutputStream os = new BufferedOutputStream(new FileOutputStream(outFile), BUFFER_SIZE)) {
                    byte[] buf = new byte[BUFFER_SIZE];
                    int len;
                    while ((len = zis.read(buf)) > 0) os.write(buf, 0, len);
                }
                zis.closeEntry();
            }

            runOnUiThread(() -> {
                Toast.makeText(this, R.string.import_success, Toast.LENGTH_SHORT).show();
                refreshStatus();
            });
        } catch (IOException e) {
            Log.e(TAG, "ZIP import failed", e);
            runOnUiThread(() -> {
                Toast.makeText(this, getString(R.string.import_failed, e.getMessage()), Toast.LENGTH_LONG).show();
                refreshStatus();
            });
        }
    }

    private String stripCommonPrefix(String name) {
        name = name.replace('\\', '/').replaceAll("^/+", "");

        if (isKnownTopLevel(name)) return name;

        int slash = name.indexOf('/');
        if (slash > 0 && slash < name.length() - 1) {
            return name.substring(slash + 1);
        }

        return name;
    }

    private static boolean isKnownTopLevel(String name) {
        return name.startsWith("main.pak") || name.startsWith("properties/") ||
               name.startsWith("Properties/") || name.startsWith("data/") ||
               name.startsWith("images/") || name.startsWith("particles/") ||
               name.startsWith("reanim/") || name.startsWith("sounds/") ||
               name.startsWith("compiled/");
    }

    private void setWorking(boolean working) {
        progressBar.setVisibility(working ? View.VISIBLE : View.GONE);
        urlInput.setEnabled(!working);
        btnDownloadImport.setEnabled(!working);
        btnLaunchGame.setEnabled(!working && hasResources());
    }
}
