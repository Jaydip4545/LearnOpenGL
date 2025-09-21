#shader vertex

#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_model;

uniform mat4 u_view;

uniform mat4 u_projection;

out vec3 vWorldPos;



void main()

{

	vec4 worldPos = u_model * vec4(aPos, 1.0);

	vWorldPos = worldPos.xyz;

	gl_Position = u_projection * u_view * worldPos;

}



#shader geometry

#version 330 core

layout(triangles) in;

layout(triangle_strip, max_vertices = 64) out;



in vec3 vWorldPos[];

out float gDepth;
out vec3 gFragPos;
out vec3 gNormal;



uniform vec3 u_cameraPos;

uniform mat4 u_view;

uniform mat4 u_projection;

uniform float u_sliceOffset;



const float EPS = 1e-7;



uniform int PLANE_COUNT;

const vec4 planes[10] = vec4[](

	vec4(1.0, 0.0, 0.0, 0.0),
	vec4(0.0, 1.0, 0.0, 0.0),
	vec4(0.0, 0.0, 1.0, 40.0),
	vec4(1.0, 0.0, 0.0, -0.5),
	vec4(1.0, 1.0, 0.0, 0.0),
	vec4(0.0, 1.0, 1.0, 40.0),
	vec4(1.0, 1.0, 1.0, 40.0),
	vec4(0.0, 0.0, 1.0, 0.0),
	vec4(0.0, 1.0, 0.0, -100.0),
	vec4(0.0, 0.0, 1.0, 39.5)
	 
	
	);




const int MAX_TRIANGLES = 64;



struct Tri {

	vec3 a;

	vec3 b;

	vec3 c;

};



vec4 normalizedPlane(vec4 plane) {

	float len = length(plane.xyz);

	if (len <= EPS) return plane;

	return plane / len;

}



float signedDistance(vec3 p, vec4 planeN) {

	return dot(vec4(p, 1.0), planeN);

}



vec3 intersectEdge(vec3 a, vec3 b, float da, float db) {

	float denom = (da - db);

	if (abs(denom) < 1e-8) return a;

	float t = da / denom;

	return mix(a, b, t);

}



vec3 calculateNormal(vec3 p1, vec3 p2, vec3 p3) {

	vec3 n = cross(p2 - p1, p3 - p1);

	float nl = length(n);

	if (nl < 1e-9) return vec3(0.0, 0.0, 1.0);

	return n / nl;

}





vec3 calculateTriangleOffset(vec3 triCenter) {

	vec3 totalOffset = vec3(0.0);


	for (int i = 0; i < PLANE_COUNT; i++) {

		vec4 planeN = normalizedPlane(planes[i]);

		float dist = signedDistance(triCenter, planeN);


		// Move away from the plane based on which side we're on

		vec3 planeNormal = normalize(planeN.xyz);

		if (dist >= 0.0) {

			// Positive side - move further in positive normal direction

			totalOffset += planeNormal * u_sliceOffset;

		}
		else {

			// Negative side - move further in negative normal direction

			totalOffset -= planeNormal * u_sliceOffset;

		}

	}


	return totalOffset;

}



void pushTriangle(inout Tri arr[MAX_TRIANGLES], inout int cnt, vec3 A, vec3 B, vec3 C, vec3 originalNormal) {

	float area2 = length(cross(B - A, C - A));

	if (area2 < 1e-6) return;


	vec3 triN = normalize(cross(B - A, C - A));

	if (dot(triN, originalNormal) < 0.0) {

		vec3 tmp = B;

		B = C;

		C = tmp;

	}


	if (cnt >= MAX_TRIANGLES) return;

	arr[cnt].a = A;

	arr[cnt].b = B;

	arr[cnt].c = C;

	cnt++;

}



void clipTriangleAgainstPlane(Tri inTri, vec4 planeN, inout Tri outArr[MAX_TRIANGLES], inout int outCount, vec3 originalNormal) {

	vec3 a = inTri.a;

	vec3 b = inTri.b;

	vec3 c = inTri.c;



	float d0 = signedDistance(a, planeN);

	float d1 = signedDistance(b, planeN);

	float d2 = signedDistance(c, planeN);



	int insideCount = 0;

	if (d0 >= -EPS) insideCount++;

	if (d1 >= -EPS) insideCount++;

	if (d2 >= -EPS) insideCount++;



	// Case: all vertices on positive side or on the plane

	if (insideCount == 3) {

		pushTriangle(outArr, outCount, a, b, c, originalNormal);

		return;

	}


	// Case: all vertices on negative side

	if (insideCount == 0) {

		pushTriangle(outArr, outCount, a, b, c, originalNormal);

		return;

	}



	// Mixed cases - triangle crosses the plane

	if (insideCount == 1) {

		vec3 inV; float dIn;

		vec3 outV1; float dOut1;

		vec3 outV2; float dOut2;



		if (d0 >= -EPS) { inV = a; dIn = d0; outV1 = b; dOut1 = d1; outV2 = c; dOut2 = d2; }

		else if (d1 >= -EPS) { inV = b; dIn = d1; outV1 = c; dOut1 = d2; outV2 = a; dOut2 = d0; }

		else { inV = c; dIn = d2; outV1 = a; dOut1 = d0; outV2 = b; dOut2 = d1; }



		vec3 i1 = intersectEdge(inV, outV1, dIn, dOut1);

		vec3 i2 = intersectEdge(inV, outV2, dIn, dOut2);



		// Positive side triangle

		pushTriangle(outArr, outCount, inV, i1, i2, originalNormal);


		// Negative side quad (as two triangles)

		pushTriangle(outArr, outCount, outV1, outV2, i1, originalNormal);

		pushTriangle(outArr, outCount, i1, outV2, i2, originalNormal);

	}

	else if (insideCount == 2) {

		vec3 outV; float dOut;

		vec3 inV1; float dIn1;

		vec3 inV2; float dIn2;



		if (d0 < -EPS) { outV = a; dOut = d0; inV1 = b; dIn1 = d1; inV2 = c; dIn2 = d2; }

		else if (d1 < -EPS) { outV = b; dOut = d1; inV1 = c; dIn1 = d2; inV2 = a; dIn2 = d0; }

		else { outV = c; dOut = d2; inV1 = a; dIn1 = d0; inV2 = b; dIn2 = d1; }



		vec3 i1 = intersectEdge(outV, inV1, dOut, dIn1);

		vec3 i2 = intersectEdge(outV, inV2, dOut, dIn2);



		// Positive side quad (as two triangles)

		pushTriangle(outArr, outCount, inV1, inV2, i1, originalNormal);

		pushTriangle(outArr, outCount, i1, inV2, i2, originalNormal);


		// Negative side triangle

		pushTriangle(outArr, outCount, outV, i1, i2, originalNormal);

	}

}



void emitTriangle(vec3 a, vec3 b, vec3 c, vec3 triNormal, vec3 offset) {
	float area2 = length(cross(b - a, c - a));
	if (area2 < 1e-6) return;

	// Calculate final vertex positions after offset
	vec3 fa = a + offset;
	vec3 fb = b + offset;
	vec3 fc = c + offset;

	// --- Emit first vertex ---
	gFragPos = fa; // <-- SET the world position for this vertex
	gDepth = distance(u_cameraPos, fa);
	gNormal = triNormal;
	gl_Position = u_projection * u_view * vec4(fa, 1.0);
	EmitVertex();

	// --- Emit second vertex ---
	gFragPos = fb; // <-- SET the world position for this vertex
	gDepth = distance(u_cameraPos, fb);
	gNormal = triNormal;
	gl_Position = u_projection * u_view * vec4(fb, 1.0);
	EmitVertex();

	// --- Emit third vertex ---
	gFragPos = fc; // <-- SET the world position for this vertex
	gDepth = distance(u_cameraPos, fc);
	gNormal = triNormal;
	gl_Position = u_projection * u_view * vec4(fc, 1.0);
	EmitVertex();

	EndPrimitive();
}


void main() {

	vec3 p0 = vWorldPos[0];

	vec3 p1 = vWorldPos[1];

	vec3 p2 = vWorldPos[2];



	vec3 originalNormal = calculateNormal(p0, p1, p2);



	// Start with the original triangle

	Tri current[MAX_TRIANGLES];

	int currentCount = 1;

	current[0].a = p0;

	current[0].b = p1;

	current[0].c = p2;



	// Process each plane sequentially

	for (int planeIdx = 0; planeIdx < PLANE_COUNT; planeIdx++) {

		Tri next[MAX_TRIANGLES];

		int nextCount = 0;

		vec4 planeN = normalizedPlane(planes[planeIdx]);



		for (int t = 0; t < currentCount; t++) {

			clipTriangleAgainstPlane(current[t], planeN, next, nextCount, originalNormal);

		}



		// Copy results back

		currentCount = nextCount;

		for (int i = 0; i < currentCount; i++) {

			current[i] = next[i];

		}



		if (currentCount == 0) break;

	}



	// Emit all resulting triangles with their appropriate offsets

	for (int i = 0; i < currentCount; i++) {

		vec3 a = current[i].a;

		vec3 b = current[i].b;

		vec3 c = current[i].c;

		vec3 triN = calculateNormal(a, b, c);


		// Calculate the center of this specific triangle piece

		vec3 triCenter = (a + b + c) / 3.0;


		// Calculate offset based on this triangle's position relative to all planes

		vec3 offset = calculateTriangleOffset(triCenter);


		emitTriangle(a, b, c, triN, offset);

	}

}



#shader fragment

#version 330 core


in vec3 gFragPos; 
in float gDepth;
in vec3 gNormal;


uniform float minDepth;
uniform float maxDepth;
uniform vec3 u_viewPos;  
uniform vec3 u_lightPos;  
uniform vec3 u_lightColor; 
uniform int phongEnabled;

out vec4 FragColor;


vec3 depthToColor(float normalizedDepth) {
	return mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), normalizedDepth);
}

void main()
{
	
	float range = maxDepth - minDepth;
	if (range <= 0.001) {
		FragColor = vec4(1.0, 0.0, 1.0, 1.0); 
		return;
	}
	float normalizedDepth = clamp((gDepth - minDepth) / range, 0.0, 1.0);
	vec3 objectColor = depthToColor(normalizedDepth);

	// 2. Calculate Phong lighting components
	// AMBIENT
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * u_lightColor;

	// DIFFUSE
	vec3 norm = normalize(gNormal);
	vec3 lightDir = normalize(u_lightPos - gFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * u_lightColor;

	// SPECULAR (the shiny highlight)
	float specularStrength = 0.5;
	float shininess = 32.0; 
	vec3 viewDir = normalize(u_viewPos - gFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = specularStrength * spec * u_lightColor;
	vec3 result;
	if(phongEnabled==1)
	{
		result = (ambient + diffuse + specular) * objectColor;
	}
	else
	{
		result = objectColor;
	}
	
	FragColor = vec4(result, 1.0);
}