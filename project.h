#pragma pack(1)

unsigned char project[] = {
	//Number of components
	0, 1,
	//Component type
	2,		//Texture
		//Texture name
		'T','e','x','t','u','r','e','0',0,
		//Number of FX
		0,


	//Number of components
	0, 2,

	//Component type
	0,		//Gradient

		//Gradient name
		'G','r','a','d','i','e','n','t','0',0,

		//Number of colors
		2,

		//Color 0: RGB + Position
		255, 255, 255,
		0,

		//Color 1: RGB + Position
		0, 0, 0,
		255,


	//Component type
	2,		//Texture

		//Texture name
		'T','e','x','t','u','r','e','0',0,

		//Number of FX
		1,

		//Active (1 bit) + FX type (7 bits)
		128,
		//Destination layer (2 bits) + Width (3 bits) + Height (3 bits)
		45,
		//Scale X (4 bits) + Scale Y (4 bits)
		0,
		//Gradient
		'G','r','a','d','i','e','n','t','0',0,


};

#pragma pack()
