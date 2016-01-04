# -*- coding: utf-8 -*-
# AOR: GT team


class ERROR_IDS(object):
	FILE_NOT_FOUND = 0
	INVALID_XML = 1
	INVALID_SCHEME = 2
	INVALID_VALUE_TYPE = 3
	INVALID_VALUE_ = 4


class ERROR_TAGS(object):
	XML = 'XML'
	SCHEME = 'SCHEME'
	VALUE = 'VALUE'


class Error(object):
	def __init__(self, code, tag, source, description):
		self.code = code
		self.tag = tag
		self.source = source
		self.description = description


def SchemeError(source, description):
	return Error(ERROR_IDS.INVALID_SCHEME, ERROR_IDS.INVALID_SCHEME, source, description)
