#version 460 core
out vec4 FragColor;

#define MAX_TEXTURE_DIFFUSE_SIZE 8
#define MAX_TEXTURE_SPECULAR_SIZE 8 
struct Material {
    int diffuseSize;
    int specularSize;
    float shininess;
    sampler2D diffuseTextures[MAX_TEXTURE_DIFFUSE_SIZE];
    sampler2D specularTextures[MAX_TEXTURE_SPECULAR_SIZE];
}; 

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
struct SpotLight {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;  
in vec2 TexCoords;
in vec3 pos;
in vec2 texcoords_first;
in vec2 texcoords_second;
in vec2 texcoords_third;
in vec2 texcoords_fourth;

uniform vec3 viewPos;
uniform Material material;

#define NR_DIRECT_LIGHTS 1
#define NR_SPOT_LIGHTS 1 
#define NR_POINT_LIGHTS 4

#if NR_DIRECT_LIGHTS != 0
uniform DirLight dirLights[NR_DIRECT_LIGHTS];
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuseTextures[0], TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuseTextures[0], TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularTextures[0], TexCoords));
    return (ambient + diffuse + specular);
}
#endif
#if NR_POINT_LIGHTS != 0
uniform PointLight pointLights[NR_POINT_LIGHTS];
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuseTextures[0], TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuseTextures[0], TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularTextures[0], TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
#endif
#if NR_SPOT_LIGHTS != 0
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuseTextures[0], TexCoords).rgb;
    
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuseTextures[0], TexCoords).rgb;  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specularTextures[0], TexCoords).rgb;  
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    
    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    return (ambient + diffuse + specular);
} 
#endif

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}
uniform float stepx;
uniform float stepy;
uniform float stepz;
struct palette {
    vec3 c0, c1, c2, c3, c4;
};


vec3 cmap( float t, palette p ) {
    //t=fract(t);
    vec3 col = vec3(0);
    col = mix( p.c0,  p.c1, smoothstep(0. , .2, t));
    col = mix( col, p.c2, smoothstep(.2, .4 , t));
    col = mix( col, p.c3, smoothstep(.4 , .6, t));
    col = mix( col, p.c4, smoothstep(.6,  .8, t));
    col = mix( col, vec3(0), smoothstep(.8, 1.,  t));
    return col;
}

palette palette1() {
    palette p;
    p.c0 = vec3(0);
    p.c1 = vec3(190,220,227)/255.;
    p.c2 = vec3(243,243,246)/255.;
    p.c3 = vec3(227,220,213)/255.;
    p.c4 = vec3(218,112, 21)/255.;
    return p;
}

palette palette2() {
    palette p; p.c0 = vec3(0,2,5)/255.;
    p.c1 = vec3(8,45,58)/255.;
    p.c2 = vec3(38,116,145)/255.;
    p.c3 = vec3(167,184,181)/255.;
    p.c4 = vec3(207,197,188)/255.;
    return p;    
}

float cosh(float val) {
  float tmp = exp(val);
  return (tmp + 1.0 / tmp) / 2.0;
}
 
float tanh(float val) {
  float tmp = exp(val);
  return (tmp - 1.0 / tmp) / (tmp + 1.0 / tmp);
}
 
float sinh(float val) {
  float tmp = exp(val);
  return (tmp - 1.0 / tmp) / 2.0;
}

vec2 cosh(vec2 val) {
  vec2 tmp = exp(val);
  return(tmp + 1.0 / tmp) / 2.0;
}
 
vec2 tanh(vec2 val) {
  vec2 tmp = exp(val);
  return (tmp - 1.0 / tmp) / (tmp + 1.0 / tmp);
}
 
vec2 sinh(vec2 val) {
  vec2 tmp = exp(val);
  return (tmp - 1.0 / tmp) / 2.0;
}

vec2 c_one() { return vec2(1., 0.); }
vec2 c_i() { return vec2(0., 1.); }

float arg(vec2 c) {
  return atan(c.y, c.x);
}

vec2 c_conj(vec2 c) {
  return vec2(c.x, -c.y);
}

vec2 c_from_polar(float r, float theta) {
  return vec2(r * cos(theta), r * sin(theta));
}

vec2 c_to_polar(vec2 c) {
  return vec2(length(c), atan(c.y, c.x));
}

/// Computes `e^(c)`, where `e` is the base of the natural logarithm.
vec2 c_exp(vec2 c) {
  return c_from_polar(exp(c.x), c.y);
}


/// Raises a floating point number to the complex power `c`.
vec2 c_exp(float base, vec2 c) {
  return c_from_polar(pow(base, c.x), c.y * log(base));
}

/// Computes the principal value of natural logarithm of `c`.
vec2 c_ln(vec2 c) {
  vec2 polar = c_to_polar(c);
  return vec2(log(polar.x), polar.y);
}

/// Returns the logarithm of `c` with respect to an arbitrary base.
vec2 c_log(vec2 c, float base) {
  vec2 polar = c_to_polar(c);
  return vec2(log(polar.r), polar.y) / log(base);
}

vec2 c_sqrt(vec2 c) {
  vec2 p = c_to_polar(c);
  return c_from_polar(sqrt(p.x), p.y/2.);
}

/// Raises `c` to a floating point power `e`.
vec2 c_pow(vec2 c, float e) {
  vec2 p = c_to_polar(c);
  return c_from_polar(pow(p.x, e), p.y*e);
}

/// Raises `c` to a complex power `e`.
vec2 c_pow(vec2 c, vec2 e) {
  vec2 polar = c_to_polar(c);
  return c_from_polar(
     pow(polar.x, e.x) * exp(-e.y * polar.y),
     e.x * polar.y + e.y * log(polar.x)
  );
}

vec2 c_mul(vec2 self, vec2 other) {
    return vec2(self.x * other.x - self.y * other.y, 
                self.x * other.y + self.y * other.x);
}

vec2 c_div(vec2 self, vec2 other) {
    float norm = length(other);
    return vec2(self.x * other.x + self.y * other.y,
                self.y * other.x - self.x * other.y)/(norm * norm);
}

vec2 c_sin(vec2 c) {
  return vec2(sin(c.x) * cosh(c.y), cos(c.x) * sinh(c.y));
}

vec2 c_cos(vec2 c) {
  // formula: cos(a + bi) = cos(a)cosh(b) - i*sin(a)sinh(b)
  return vec2(cos(c.x) * cosh(c.y), -sin(c.x) * sinh(c.y));
}

vec2 c_tan(vec2 c) {
  vec2 c2 = 2. * c;
  return vec2(sin(c2.x), sinh(c2.y))/(cos(c2.x) + cosh(c2.y));
}

vec2 c_atan(vec2 c) {
  // formula: arctan(z) = (ln(1+iz) - ln(1-iz))/(2i)
  vec2 i = c_i();
  vec2 one = c_one();
  vec2 two = one + one;
  if (c == i) {
    return vec2(0., 1./0.0);
  } else if (c == -i) {
    return vec2(0., -1./0.0);
  }

  return c_div(
    c_ln(one + c_mul(i, c)) - c_ln(one - c_mul(i, c)),
    c_mul(two, i)
  );
}

vec2 c_asin(vec2 c) {
 // formula: arcsin(z) = -i ln(sqrt(1-z^2) + iz)
  vec2 i = c_i(); vec2 one = c_one();
  return c_mul(-i, c_ln(
    c_sqrt(c_one() - c_mul(c, c)) + c_mul(i, c)
  ));
}

vec2 c_acos(vec2 c) {
  // formula: arccos(z) = -i ln(i sqrt(1-z^2) + z)
  vec2 i = c_i();

  return c_mul(-i, c_ln(
    c_mul(i, c_sqrt(c_one() - c_mul(c, c))) + c
  ));
}

vec2 c_sinh(vec2 c) {
  return vec2(sinh(c.x) * cos(c.y), cosh(c.x) * sin(c.y));
}

vec2 c_cosh(vec2 c) {
  return vec2(cosh(c.x) * cos(c.y), sinh(c.x) * sin(c.y));
}

vec2 c_tanh(vec2 c) {
  vec2 c2 = 2. * c;
  return vec2(sinh(c2.x), sin(c2.y))/(cosh(c2.x) + cos(c2.y));
}

vec2 c_asinh(vec2 c) {
  // formula: arcsinh(z) = ln(z + sqrt(1+z^2))
  vec2 one = c_one();
  return c_ln(c + c_sqrt(one + c_mul(c, c)));
}

vec2 c_acosh(vec2 c) {
  // formula: arccosh(z) = 2 ln(sqrt((z+1)/2) + sqrt((z-1)/2))
  vec2 one = c_one();
  vec2 two = one + one;
  return c_mul(two,
      c_ln(
        c_sqrt(c_div((c + one), two)) + c_sqrt(c_div((c - one), two))
      ));
}

vec2 c_atanh(vec2 c) {
  // formula: arctanh(z) = (ln(1+z) - ln(1-z))/2
  vec2 one = c_one();
  vec2 two = one + one;
  if (c == one) {
      return vec2(1./0., vec2(0.));
  } else if (c == -one) {
      return vec2(-1./0., vec2(0.));
  }
  return c_div(c_ln(one + c) - c_ln(one - c), two);
}

vec2 c_rem(vec2 c, vec2 modulus) {
  vec2 c0 = c_div(c, modulus);
  // This is the gaussian integer corresponding to the true ratio
  // rounded towards zero.
  vec2 c1 = vec2(c0.x - mod(c0.x, 1.), c0.y - mod(c0.y, 1.));
  return c - c_mul(modulus, c1);
}

vec2 c_inv(vec2 c) {
  float norm = length(c);
	return vec2(c.x, -c.y) / (norm * norm);
}

vec2 c_pow2(vec2 z) {
    return c_mul(z,z);
}


vec2 fract_ship(vec2 z) {
	z = abs(z);
	z = c_pow2(z);
	return z;
}
vec2 fract_weird(vec2 z) {
	z = fract_ship(z);
	z = fract(z);
	return z;
}
vec2 fract_mandelbrot(vec2 z) {
	z = c_pow2(z);
	return z;
}

vec2 fract_sinz2(vec2 z) {
	z = fract_mandelbrot(z);
	z = c_sin(z);
	return z;
}
vec2 fract_cosz2(vec2 z) {
	z = fract_mandelbrot(z);
	z = c_cos(z);
	return z;
}

double b = 2;
float tex_scale = 0.2;
float t = 0;
int limit = 256;
uniform float time;

void main() {
    vec2 c = vec2(TexCoords - 0.5) / tex_scale;
	//c = 0.1*(c - vec2(.2,0)) - vec2(1.74,0.035); // ship
    
    vec2 z;
    for(int i = 0; i < limit; i++) {
		z = fract_sinz2(z);
		z = fract_cosz2(z);
		z = fract_sinz2(z);
		z = fract_cosz2(z);
        z += c;
        if(dot(z,z) > b*b) break;
        t++;
    }


    FragColor = 1 - vec4(1 - sin(1 - t / limit));
    
}