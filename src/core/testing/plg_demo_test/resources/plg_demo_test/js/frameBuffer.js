/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCanvas3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

Qt.include("gl-matrix.js")

//
// Draws a cube that has the Qt logo as decal texture on each face in to a texture.
// That texture is used as the texture for drawing another cube on the screen.
//

var gl;

var rttFramebuffer;
var rttTexture;
var rttWidth = 512;
var rttHeight = 512;

var cubeTexture = 0;

var vertexPositionAttribute;
var textureCoordAttribute;
var vertexNormalAttribute;
var vertexColorAttribute;

var pMatrixUniform;
var mvMatrixUniform;
var nUniform;

var canvas3d;
var isLogEnabled = false;

function log(message) {
    if (isLogEnabled)
        console.log(message)
}

function initializeGL(canvas, textureLoader) {
    canvas3d = canvas
    try {
        // Get the OpenGL context object that represents the API we call
        gl = canvas.getContext("canvas3d", {depth:true, antialias:true});

        // Setup the OpenGL state
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);
        gl.cullFace(gl.BACK);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);

        // Initialize the shader program
        initShaders();

        // Initialize vertex and color buffers
        initBuffers();

        // Load the Qt logo as texture
        var qtLogoImage = TextureImageFactory.newTexImage();
        qtLogoImage.imageLoaded.connect(function() {
            cubeTexture = gl.createTexture();
            cubeTexture.name = "CubeTexture";
            gl.bindTexture(gl.TEXTURE_2D, cubeTexture);
            gl.texImage2D(gl.TEXTURE_2D,    // target
                          0,                // level
                          gl.RGBA,          // internalformat
                          gl.RGBA,          // format
                          gl.UNSIGNED_BYTE, // type
                          qtLogoImage);    // pixels

            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
            gl.generateMipmap(gl.TEXTURE_2D);
        });
        qtLogoImage.imageLoadingFailed.connect(function() {
            console.log("Texture load FAILED, "+qtLogoImage.errorString);
        });
        qtLogoImage.src = "../viewports/viewport4.jpg";

        // Create the framebuffer object
        rttFramebuffer = gl.createFramebuffer();
        rttFramebuffer.name = "OffscreenRenderTarget";
        gl.bindFramebuffer(gl.FRAMEBUFFER, rttFramebuffer);

        // Create the texture
        rttTexture = gl.createTexture();
        rttTexture.name = "OffscreenRenderTargetTexture";
        gl.bindTexture(gl.TEXTURE_2D, rttTexture);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
        gl.texImage2D(gl.TEXTURE_2D, 0,
                      gl.RGBA, rttWidth, rttHeight,
                      0, gl.RGBA, gl.UNSIGNED_BYTE,
                      null);
        gl.generateMipmap(gl.TEXTURE_2D);

        // Bind the texture as color attachment, create and bind a depth buffer
        gl.framebufferTexture2D(gl.FRAMEBUFFER,
                                gl.COLOR_ATTACHMENT0,
                                gl.TEXTURE_2D, rttTexture, 0);
        var renderbuffer = gl.createRenderbuffer();
        gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);
        gl.renderbufferStorage(gl.RENDERBUFFER,
                               gl.DEPTH_COMPONENT16,
                               rttWidth, rttHeight);
        gl.framebufferRenderbuffer(gl.FRAMEBUFFER,
                                   gl.DEPTH_ATTACHMENT,
                                   gl.RENDERBUFFER, renderbuffer);
        gl.bindTexture(gl.TEXTURE_2D, 0);
        gl.bindRenderbuffer(gl.RENDERBUFFER, 0);
        gl.bindFramebuffer(gl.FRAMEBUFFER, 0);
    } catch(e) {
        console.log("initializeGL FAILURE!");
        console.log(""+e);
        console.log(""+e.message);
    }
}

function degToRad(degrees) {
    return degrees * Math.PI / 180;
}


function paintCube(canvas, position) {

    var mvMatrix = mat4.create();
    var pMatrix  = mat4.create();
    var nMatrix  = mat4.create();

    // Calculate and set matrix uniforms
    mat4.perspective(pMatrix, degToRad(90), canvas.width / canvas.height, 0.1, 100.0);
    gl.uniformMatrix4fv(pMatrixUniform, false, pMatrix);

    mat4.identity(mvMatrix);
    
    mat4.translate(mvMatrix, mvMatrix, [position[0],
                                        position[1],
                                        position[2]]);

    log(mat4.str(mvMatrix));
    //mat4.rotate(mvMatrix, mvMatrix, degToRad(canvas.xRotAnim), [0, 1, 0]);
    
    gl.uniformMatrix4fv(mvMatrixUniform, false, mvMatrix);
    
    mat4.invert(nMatrix, mvMatrix);
    mat4.transpose(nMatrix, nMatrix);
    
    gl.uniformMatrix4fv(nUniform, false, nMatrix);

    // Draw the on-screen cube
    gl.drawElements(gl.TRIANGLES, 36, gl.UNSIGNED_SHORT, 0);
}

function paintGL(canvas, positions) {
    var mvMatrix = mat4.create();
    var pMatrix  = mat4.create();
    var nMatrix  = mat4.create();

    // bind the FBO and setup viewport
    gl.bindFramebuffer(gl.FRAMEBUFFER, rttFramebuffer);
    gl.viewport(0, 0, rttWidth, rttHeight);

    gl.clearColor(0.95, 0.95, 0.95, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // Bind the loaded texture
    gl.bindTexture(gl.TEXTURE_2D, cubeTexture);

    // Calculate and set matrix uniforms
    mat4.perspective(pMatrix, degToRad(45), rttWidth / rttHeight, 0.1, 100.0);
    gl.uniformMatrix4fv(pMatrixUniform, false, pMatrix);

    mat4.identity(mvMatrix);
    mat4.translate(mvMatrix, mvMatrix, [0, 0, -5.0]);
    mat4.rotate(mvMatrix, mvMatrix, degToRad(canvas.xRotSlider), [0, 1, 0]);
    mat4.rotate(mvMatrix, mvMatrix, degToRad(canvas.yRotSlider), [1, 0, 0]);
    mat4.rotate(mvMatrix, mvMatrix, degToRad(canvas.zRotSlider), [0, 0, 1]);
    gl.uniformMatrix4fv(mvMatrixUniform, false, mvMatrix);

    mat4.invert(nMatrix, mvMatrix);
    mat4.transpose(nMatrix, nMatrix);
    gl.uniformMatrix4fv(nUniform, false, nMatrix);

    // Draw the cube to the FBO
    gl.drawElements(gl.TRIANGLES, 36, gl.UNSIGNED_SHORT, 0);

    // Bind the render-to-texture and generate mipmaps
    gl.bindTexture(gl.TEXTURE_2D, rttTexture);
    gl.generateMipmap(gl.TEXTURE_2D);

    // Bind default framebuffer and setup viewport accordingly
    gl.bindFramebuffer(gl.FRAMEBUFFER, 0);
    gl.viewport(0, 0,
                canvas.width * canvas.devicePixelRatio,
                canvas.height * canvas.devicePixelRatio);

    gl.clearColor(0.50, 0.50, 0.50, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    for(var i = 0; i < positions.length; i++) {
        paintCube(canvas, positions[i]) 
    }
}

function resizeGL(canvas)
{
    var pixelRatio = canvas.devicePixelRatio;
    canvas.pixelSize = Qt.size(canvas.width * pixelRatio,
                               canvas.height * pixelRatio);
}

function initBuffers()
{
    log("        cubeVertexPositionBuffer");
    var cubeVertexPositionBuffer = gl.createBuffer();
    cubeVertexPositionBuffer.name = "cubeVertexPositionBuffer";
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVertexPositionBuffer);
    gl.bufferData(
                gl.ARRAY_BUFFER,
                new Float32Array([// Front face
                                        -1.0, -1.0,  1.0,
                                        1.0, -1.0,  1.0,
                                        1.0,  1.0,  1.0,
                                        -1.0,  1.0,  1.0,

                                        // Back face
                                        -1.0, -1.0, -1.0,
                                        -1.0,  1.0, -1.0,
                                        1.0,  1.0, -1.0,
                                        1.0, -1.0, -1.0,

                                        // Top face
                                        -1.0,  1.0, -1.0,
                                        -1.0,  1.0,  1.0,
                                        1.0,  1.0,  1.0,
                                        1.0,  1.0, -1.0,

                                        // Bottom face
                                        -1.0, -1.0, -1.0,
                                        1.0, -1.0, -1.0,
                                        1.0, -1.0,  1.0,
                                        -1.0, -1.0,  1.0,

                                        // Right face
                                        1.0, -1.0, -1.0,
                                        1.0,  1.0, -1.0,
                                        1.0,  1.0,  1.0,
                                        1.0, -1.0,  1.0,

                                        // Left face
                                        -1.0, -1.0, -1.0,
                                        -1.0, -1.0,  1.0,
                                        -1.0,  1.0,  1.0,
                                        -1.0,  1.0, -1.0
                                       ]),
                gl.STATIC_DRAW);

    gl.enableVertexAttribArray(vertexPositionAttribute);
    gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);

    log("        cubeVertexIndexBuffer");
    var cubeVertexIndexBuffer = gl.createBuffer();
    cubeVertexIndexBuffer.name = "cubeVertexIndexBuffer";
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVertexIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,
                  new Uint16Array([
                                            0,  1,  2,      0,  2,  3,    // front
                                            4,  5,  6,      4,  6,  7,    // back
                                            8,  9,  10,     8,  10, 11,   // top
                                            12, 13, 14,     12, 14, 15,   // bottom
                                            16, 17, 18,     16, 18, 19,   // right
                                            20, 21, 22,     20, 22, 23    // left
                                        ]),
                  gl.STATIC_DRAW);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVertexIndexBuffer);

    log("        cubeVerticesTextureCoordBuffer");
    var cubeVerticesTextureCoordBuffer = gl.createBuffer();
    cubeVerticesTextureCoordBuffer.name = "cubeVerticesTextureCoordBuffer";
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesTextureCoordBuffer);
    var textureCoordinates = [
                // Front
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0,
                // Back
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0,
                // Top
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0,
                // Bottom
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0,
                // Right
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0,
                // Left
                1.0,  0.0,
                0.0,  0.0,
                0.0,  1.0,
                1.0,  1.0
            ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(textureCoordinates),
                  gl.STATIC_DRAW);
    gl.enableVertexAttribArray(textureCoordAttribute);
    gl.vertexAttribPointer(textureCoordAttribute, 2, gl.FLOAT, false, 0, 0);

    var cubeVerticesNormalBuffer = gl.createBuffer();
    cubeVerticesNormalBuffer.name = "cubeVerticesNormalBuffer";
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesNormalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
                                                              // Front
                                                              0.0,  0.0,  1.0,
                                                              0.0,  0.0,  1.0,
                                                              0.0,  0.0,  1.0,
                                                              0.0,  0.0,  1.0,

                                                              // Back
                                                              0.0,  0.0, -1.0,
                                                              0.0,  0.0, -1.0,
                                                              0.0,  0.0, -1.0,
                                                              0.0,  0.0, -1.0,

                                                              // Top
                                                              0.0,  1.0,  0.0,
                                                              0.0,  1.0,  0.0,
                                                              0.0,  1.0,  0.0,
                                                              0.0,  1.0,  0.0,

                                                              // Bottom
                                                              0.0, -1.0,  0.0,
                                                              0.0, -1.0,  0.0,
                                                              0.0, -1.0,  0.0,
                                                              0.0, -1.0,  0.0,

                                                              // Right
                                                              1.0,  0.0,  0.0,
                                                              1.0,  0.0,  0.0,
                                                              1.0,  0.0,  0.0,
                                                              1.0,  0.0,  0.0,

                                                              // Left
                                                              -1.0,  0.0,  0.0,
                                                              -1.0,  0.0,  0.0,
                                                              -1.0,  0.0,  0.0,
                                                              -1.0,  0.0,  0.0
                                                          ]), gl.STATIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesNormalBuffer);
    gl.vertexAttribPointer(vertexNormalAttribute, 3, gl.FLOAT, false, 0, 0);
}

function initShaders()
{
    var vertexShader = getShader(gl,
                                 "attribute highp vec3 aVertexNormal;   \
                                  attribute highp vec3 aVertexPosition; \
                                  attribute highp vec2 aTextureCoord;   \

                                  uniform highp mat4 uNormalMatrix;     \
                                  uniform mat4 uMVMatrix;               \
                                  uniform mat4 uPMatrix;                \

                                  varying mediump vec4 vColor;          \
                                  varying highp vec2 vTextureCoord;     \
                                  varying highp vec3 vLighting;         \

                                  void main(void) {                     \
                                      gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 1.0);                      \
                                      vTextureCoord = aTextureCoord;                                                        \
                                      highp vec3 ambientLight = vec3(0.5, 0.5, 0.5);                                        \
                                      highp vec3 directionalLightColor = vec3(0.75, 0.75, 0.75);                             \
                                      highp vec3 directionalVector = vec3(0.85, 0.8, 0.75);                                 \
                                      highp vec4 transformedNormal = uNormalMatrix * vec4(aVertexNormal, 1.0);              \
                                      highp float directional = max(dot(transformedNormal.xyz, directionalVector), 0.0);    \
                                      vLighting = ambientLight + (directionalLightColor * directional);                     \
                                  }", gl.VERTEX_SHADER);
    var fragmentShader = getShader(gl,
                                   "varying highp vec2 vTextureCoord;   \
                                    varying highp vec3 vLighting;       \

                                    uniform sampler2D uSampler;         \

                                    void main(void) {                   \
                                        mediump vec3 texelColor = texture2D(uSampler, vec2(vTextureCoord.s, vTextureCoord.t)).rgb;  \
                                        gl_FragColor = vec4(texelColor * vLighting, 1.0);                                       \
                                    }", gl.FRAGMENT_SHADER);

    var shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);

    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
        console.log("Could not initialise shaders");
        console.log(gl.getProgramInfoLog(shaderProgram));
    }

    gl.useProgram(shaderProgram);

    // look up where the vertex data needs to go.
    vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
    gl.enableVertexAttribArray(vertexPositionAttribute);
    textureCoordAttribute = gl.getAttribLocation(shaderProgram, "aTextureCoord");
    gl.enableVertexAttribArray(textureCoordAttribute);
    vertexNormalAttribute =gl.getAttribLocation(shaderProgram, "aVertexNormal");
    gl.enableVertexAttribArray(vertexNormalAttribute);

    pMatrixUniform  = gl.getUniformLocation(shaderProgram, "uPMatrix");
    mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
    nUniform = gl.getUniformLocation(shaderProgram, "uNormalMatrix");

    var textureSamplerUniform = gl.getUniformLocation(shaderProgram, "uSampler")
    gl.activeTexture(gl.TEXTURE0);
    gl.uniform1i(textureSamplerUniform, 0);
}

function getShader(gl, str, type) {
    var shader = gl.createShader(type);
    gl.shaderSource(shader, str);
    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        console.log("JS:Shader compile failed");
        console.log(gl.getShaderInfoLog(shader));
        return null;
    }

    return shader;
}

function glUnProject(objx, objy, objz, modelview, projection, viewport, windowCoordinate)
{
    //Transformation vectors
    var fTempo;
    //Modelview transform
    fTempo[0]=modelview[0]*objx+modelview[4]*objy+modelview[8]*objz+modelview[12];  //w is always 1
    fTempo[1]=modelview[1]*objx+modelview[5]*objy+modelview[9]*objz+modelview[13];
    fTempo[2]=modelview[2]*objx+modelview[6]*objy+modelview[10]*objz+modelview[14];
    fTempo[3]=modelview[3]*objx+modelview[7]*objy+modelview[11]*objz+modelview[15];
    //Projection transform, the final row of projection matrix is always [0 0 -1 0]
    //so we optimize for that.
    fTempo[4]=projection[0]*fTempo[0]+projection[4]*fTempo[1]+projection[8]*fTempo[2]+projection[12]*fTempo[3];
    fTempo[5]=projection[1]*fTempo[0]+projection[5]*fTempo[1]+projection[9]*fTempo[2]+projection[13]*fTempo[3];
    fTempo[6]=projection[2]*fTempo[0]+projection[6]*fTempo[1]+projection[10]*fTempo[2]+projection[14]*fTempo[3];
    fTempo[7]=-fTempo[2];
    //The result normalizes between -1 and 1
    if(fTempo[7]==0.0)        //The w value
        return 0;
    fTempo[7]=1.0/fTempo[7];
    //Perspective division
    fTempo[4]*=fTempo[7];
    fTempo[5]*=fTempo[7];
    fTempo[6]*=fTempo[7];
    //Window coordinates
    //Map x, y to range 0-1
    windowCoordinate[0]=(fTempo[4]*0.5+0.5)*viewport[2]+viewport[0];
    windowCoordinate[1]=(fTempo[5]*0.5+0.5)*viewport[3]+viewport[1];
    //This is only correct when glDepthRange(0.0, 1.0)
    windowCoordinate[2]=(1.0+fTempo[6])*0.5;  //Between 0 and 1
    return 1;
}

function getSelectedObject(mouseX, mouseY)
{
    var x = mouseX;
    var y = canvas3d.height - mouseY;
    console.log("===screen point: " + x + " : " + y + " =====");


    return 0;
}