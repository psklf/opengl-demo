/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.gles3jni;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class GLES3JNIView extends GLSurfaceView {
    private static final String TAG = "GLES3JNI";
    private static final boolean DEBUG = true;

    public GLES3JNIView(Context context) {
        super(context);
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        setEGLContextClientVersion(3);
        setRenderer(new Renderer());
        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    private  class Renderer implements GLSurfaceView.Renderer {
        public void onDrawFrame(GL10 gl) {
            GLES3JNILib.step();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            GLES3JNILib.resize(width, width);


            BitmapFactory.Options opt = new BitmapFactory.Options();
            opt.inScaled = false;
            Bitmap bmp = BitmapFactory.decodeResource(GLES3JNIView.this.getResources(),
                    R.mipmap.color, opt);
//            Bitmap bmp_depth = BitmapFactory.decodeResource(GLES3JNIView.this.getResources(),
//                    R.mipmap.depth, opt);

            Bitmap bmp_metal_albedo = BitmapFactory.decodeResource(GLES3JNIView.this.getResources(),
                    R.mipmap.metal_albedo, opt);


            GLES3JNILib.set2DTexture(bmp_metal_albedo,
                    bmp_metal_albedo.getWidth(), bmp_metal_albedo.getHeight());
//            GLES3JNILib.setDepthTexture(bmp_depth, bmp_depth.getWidth(), bmp_depth.getHeight());

            bmp.recycle();
            bmp_metal_albedo.recycle();
//            bmp_depth.recycle();
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            GLES3JNILib.init();
        }
    }
}
