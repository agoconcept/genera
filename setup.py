from distutils.core import setup, Extension

generador = Extension("_genera",
                    sources = [
					"random.cpp",
					"vector.cpp",
					"genera.cpp",
					"color.cpp",
					"component.cpp",
					"fx.cpp",
					"gradient.cpp",
					"layer.cpp",
					"curve.cpp",
					"texture.cpp",
					"model.cpp",
					"random_wrap.cpp",
					"vector_wrap.cpp",
					"genera_wrap.cpp",
					"color_wrap.cpp",
					"component_wrap.cpp",
					"fx_wrap.cpp",
					"gradient_wrap.cpp",
					"layer_wrap.cpp",
					"curve_wrap.cpp",
					"texture_wrap.cpp",
					"model_wrap.cpp"])

setup (name = "Genera",
       version = "1.0",
       description = "Concept Genera",
       ext_modules = [generador])

