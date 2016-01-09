from distutils.core import setup, Extension

MOD = 'Matrix.GPIO'
setup(name          = MOD, 
	  version       = '0.0.1',
	  url           = 'http://www.friendlyarm.com',
	  author        = 'FriendlyARM',
	  author_email  = 'support@friendlyarm.com',
	  description   = 'A module to control Matrix related boards GPIO',
	  keywords      = 'matirx gpio',
	  packages      = ['Matrix'],
	  ext_modules   = [Extension(MOD, sources=['gpio.c', 'common.c', 'constants.c'])])
