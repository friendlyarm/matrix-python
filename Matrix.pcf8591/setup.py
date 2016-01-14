from distutils.core import setup, Extension

MOD = 'Matrix.pcf8591'
setup(name          = MOD, 
	  version       = '0.0.1',
	  url           = 'http://www.friendlyarm.com',
	  author        = 'FriendlyARM',
	  author_email  = 'support@friendlyarm.com',
	  description   = 'A module to control Matrix pcf8591',
	  keywords      = 'matirx pcf8591',
	  packages      = ['Matrix'],
	  ext_modules   = [Extension(MOD, sources=['pcf8591.c', 'common.c'])])
