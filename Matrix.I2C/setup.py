from distutils.core import setup, Extension

MOD = 'Matrix.I2C'
setup(name          = MOD, 
	  version       = '0.0.1',
	  url           = 'http://www.friendlyarm.com',
	  author        = 'FriendlyARM',
	  author_email  = 'support@friendlyarm.com',
	  description   = 'A module to control Matrix I2C',
	  keywords      = 'matirx I2C',
	  packages      = ['Matrix'],
	  ext_modules   = [Extension(MOD, sources=['I2C.c'])])
