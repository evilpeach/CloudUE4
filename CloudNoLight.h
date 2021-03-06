float3 color = float3(0,0,0);
//float3 sun_direction = normalize( float3(0.6,0.45,-0.8) );
//float3 SunPower = float3(1.0,0.9,0.6) * 750.0;
//float3 LOW_SCATTER = float3(1.0, 0.7, 0.5);
float mu = dot(sun_direction, dir);
float4 test = float4(0,0,0,0);

float texture1_r_perlin_low     =  Low.r;//0.3;
float texture1_r_perlin_high    =  High.r;//1.4;
float texture1_r_worley_low     =  Low.g;//-0.3;
float texture1_r_worley_high    =  High.g;//1.3;
float texture1_gba_worley_low   =  Low.b;//-0.4;
float texture1_gba_worley_high  =  High.b;//1.0;
float perlin_to_worley_ratio    =  0.4;

//skyRay

//nbSample = RaymarchStep;
float3 p = org + distToAtmStart * dir;
float stepS = (distToAtmEnd - distToAtmStart) / RaymarchStep;
float T = 1;
//phaseFuntion
p += dir * stepS * hashSky;
if(dir.z < yThreshold){
	for(int i = 0; i < RaymarchStep; i++){
		float cloudHeightIn;
		float density = 0.0;
		
		///////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////Sample clouds/////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////
		
		float atmosHeight = length(p - center) - EarthRadius;
		cloudHeightIn = clamp((atmosHeight-CloudStart)/(CloudHeight), 0.0, 1.0);
		
		float largeWeather = clamp((WeatherTex.SampleLevel(WeatherTexSampler, -0.0000001*largeWeatherScale*p.yx + (SpeedW*Time), 0.0).x-0.03)*10.0, 0.0, 2.0);
		float weather = largeWeather*max(0.0, WeatherTex.SampleLevel(WeatherTexSampler, 0.0000001*WeatherScale*p.yx, 0.0).x - 0.01) /0.72;
		weather *= smoothstep(ws.x, ws.y, cloudHeightIn) * smoothstep(ws.z, ws.w, cloudHeightIn);
		float cloudShape = pow(weather, 0.3+1.5*smoothstep(0.2, 0.5, cloudHeightIn));
		//if(cloudShape <= 0.0) return 0.0;
		if(cloudShape > 0.0){
			
			float3 xyz = p * 0.00001 * FPerlin + (SpeedN * Time);

			float perlin_r = get_perlin_7_octaves(xyz, 4.0);
			float worley_r = get_worley_3_octaves(xyz, 6.0);
			
			perlin_r = set_range(perlin_r, texture1_r_perlin_low, texture1_r_perlin_high);
			worley_r = set_range(worley_r, texture1_r_worley_low, texture1_r_worley_high);
			
			float f_FBM = dilate_perlin_worley(perlin_r, worley_r, perlin_to_worley_ratio);
			
			float den = max(0.0, cloudShape - PWDen * (1 - f_FBM));
			
			//if(den <= 0.0) return 0.0;
			if(den > 0.0){
				
				
				xyz = p * 0.00001 * FWorley;
				
				float worley_b = get_worley_3_octaves(xyz, 8.0);
				worley_b = set_range(worley_b, texture1_gba_worley_low, texture1_gba_worley_high);
				f_FBM = worley_b;
				
				den = max(0.0, den - WDen * (1 - f_FBM));
				density = largeWeather*0.2*min(1.0, 5.0*den) * (cloudHeightIn + B) * CloudDensity; 
				
			}else density = 0.0;
		}else density = 0.0;
		
		//color = float3(density, density, density);
		//density = clamp(density , 0.0,  1.0);
		if(density > 0.0){
			
			//////////////////////////////////////////LIGHT//////////////////////////////////////////////////////////
			//float zMaxl = 600;
			float LightDensity = 0;
			float stepL = zMaxl/LightStep;
			float lighRayDen = 0.0;    
			float3 pL = p;
			pL += sun_direction*stepL*hashSky;//*hash(dot(p, vec3(12.256, 2.646, 6.356)) + iTime);
			for(int j = 0; j < LightStep; j++){
				//////sample ///////
				float3 pLL = pL + sun_direction * float(j) * stepL * gled;
				int3 randpos = int3(Parameters.SvPosition.xy, View.StateFrameIndexMod8);
				float3 rand = float3(Rand3DPCG16(randpos).x, Rand3DPCG16(randpos).y, 0) / 0xffff;
				pLL += rand * stepL * gled * momo;
				
				float atmosHeightLight = length(pLL - center) - EarthRadius;
				float cloudHeightLight = clamp((atmosHeightLight-CloudStart)/(CloudHeight), 0.0, 1.0);
				//pLL.z += Time*10.3;
				largeWeather = clamp((WeatherTex.SampleLevel(WeatherTexSampler, -0.0000001*largeWeatherScale*pLL.yx + (SpeedW*Time), 0.0).x-0.03)*10.0, 0.0, 2.0);
				
				//pLL.x += Time*8.3;
				weather = largeWeather*max(0.0, WeatherTex.SampleLevel(WeatherTexSampler, 0.0000001*WeatherScale*pLL.yx, 0.0).x - 0.01) /0.72;
				weather *= smoothstep(ws.x, ws.y, cloudHeightLight) * smoothstep(ws.z, ws.w, cloudHeightLight);
				cloudShape = pow(weather, 0.3+1.5*smoothstep(0.2, 0.5, cloudHeightLight));
				//if(cloudShape <= 0.0) return 0.0;
				if(cloudShape > 0.0){
					
					
					float3 xyz = pLL * 0.00001 * FPerlin + (SpeedN * Time);
			
					float perlin_r = get_perlin_7_octaves(xyz, 4.0);
					float worley_r = get_worley_3_octaves(xyz, 6.0);
					
					perlin_r = set_range(perlin_r, texture1_r_perlin_low, texture1_r_perlin_high);
					worley_r = set_range(worley_r, texture1_r_worley_low, texture1_r_worley_high);
					
					float f_FBM = dilate_perlin_worley(perlin_r, worley_r, perlin_to_worley_ratio);
					
					float den = max(0.0, cloudShape - PWDen * (1 - f_FBM));
					
					//if(den <= 0.0) return 0.0;
					if(den > 0.0){
					
						xyz = pLL * 0.00001 * FWorley;
						
						float worley_b = get_worley_3_octaves(xyz, 8.0);
						worley_b = set_range(worley_b, texture1_gba_worley_low, texture1_gba_worley_high);
						f_FBM = worley_b;
						den = max(0.0, den - WDen * (1 - f_FBM));
						LightDensity = largeWeather*0.2*min(1.0, 5.0*den) * (cloudHeightLight + BLight) * CloudDensity;						
					}else LightDensity = 0.0;
				}else LightDensity = 0.0;
				
				lighRayDen += LightDensity;
			}
			
			lighRayDen *= ShadowDensity;
			float scatterAmount = lerp(0.008, 1.0, smoothstep(0.96, 0.0, mu));
			float beersLaw = exp(-stepL*lighRayDen);// + 0.5*scatterAmount*exp(-0.1*stepL*lighRayDen) + scatterAmount*0.4*exp(-0.02*stepL*lighRayDen);
			float powder = (1 - beersLaw * beersLaw) * DarkOutliner;
			float intensity = beersLaw * henyey * (powder + 1.0)*0.5;// * lerp(0.05 + 1.5*pow(min(1.0, density*8.5), 0.3+5.5*cloudHeightIn), 1.0, clamp(lighRayDen*0.4, 0.0, 1.0));
			//end*/

			float3 ambient = (0.5 + 0.6*cloudHeightIn)*CloudBaseColor + CloudTopColor * ( 1.0 - max(0.0, 1.0-2.0*cloudHeightIn));
			float3 radiance = AmbientScale * ambient + SunPower*intensity;
			radiance*=density;
			color += T*(radiance - radiance*exp(-density*stepS)) / density;
			T *= exp(-density*stepS);
			if(T <= TMax) break;
		}
		
		p += dir*stepS;
	}
	
	//float fogPhase = henyey;//0.5*HenyeyGreenstein(mu, 0.7)+0.5*HenyeyGreenstein(mu, -0.6);    
    //test = float4(lerp(0.1*LOW_SCATTER*SunPower+10.0*float3(0.55, 0.8, 1.0), color, exp(-0.0003*fogDistance)), 1.0) * 0.05;
	
}

if(Depth <= 0.2) return float4(0,0,0,1);
else return float4(color,T);
//return test;