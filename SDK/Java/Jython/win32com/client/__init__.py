
import sys
import org.isis.jaut
jaut = org.isis.jaut

CLSCTX_INPROC_SERVER = 1
CLSCTX_INPROC_HANDLER = 2
CLSCTX_LOCAL_SERVER = 4
CLSCTX_INPROC = 3
CLSCTX_SERVER = 5
CLSCTX_ALL = 7
CLSCTX_INPROC_SERVER16 = 8
CLSCTX_REMOTE_SERVER = 16
CLSCTX_INPROC_HANDLER16 = 32
CLSCTX_RESERVED1 = 64
CLSCTX_RESERVED2 = 128
CLSCTX_RESERVED3 = 256
CLSCTX_RESERVED4 = 512
CLSCTX_NO_CODE_DOWNLOAD = 1024
CLSCTX_RESERVED5 = 2048
CLSCTX_NO_CUSTOM_MARSHAL = 4096
CLSCTX_ENABLE_CODE_DOWNLOAD = 8192
CLSCTX_NO_FAILURE_LOG = 16384
CLSCTX_DISABLE_AAA = 32768
CLSCTX_ENABLE_AAA = 65536
CLSCTX_FROM_DEFAULT_CONTEXT = 131072

_argkludge = { 'GetXMLInfo': (jaut.Variant.VT_BSTR, jaut.Variant.VT_BSTR, jaut.Variant.VT_NULL, jaut.Variant.VT_BSTR, jaut.Variant.VT_BSTR),
'QueryParadigm': (jaut.Variant.VT_BSTR, jaut.Variant.VT_NULL),
'OpenObj': (jaut.Variant.VT_UNKNOWN,),
'CreateTerritory': (jaut.Variant.VT_NULL, jaut.Variant.VT_UNKNOWN),
}
_getterkludge = { 'Item': 1,
'Status': 1,
'ObjectByPath': 1,
'LegalChildFolderByName': 1,
'DefinedFCOByName': 1,
'RoleByName': 1,
}

_debug = False
_dbgout = sys.stderr

def _translate_java(v):
	if isinstance(v, jaut.Dispatch):
		return JautDispatch(v)
	return v

_vt_falses = ( jaut.Variant.VT_NULL, jaut.Variant.VT_EMPTY )
def __nonzero__(self):
    vt = self.getVartype()
    if vt in _vt_falses:
        return False
    return True
#org.isis.jaut.Variant.__nonzero__ = __nonzero__
del __nonzero__

def _translate_variant(v):
	if v is None:
		return None
	vt = v.getVartype()
	# FIXME: just use toObject() ?
	if _debug:
		_dbgout.write('_translate_variant: type ' + str(vt) + "\n")
	if vt == jaut.Variant.VT_BSTR | jaut.Variant.VT_ARRAY:
		arr = v.getStringArray()
		return list(arr)
	elif vt == jaut.Variant.VT_BSTR or vt == (jaut.Variant.VT_BSTR | jaut.Variant.VT_BYREF):
		return v.toString()
	elif vt == jaut.Variant.VT_DISPATCH | jaut.Variant.VT_BYREF:
		return JautDispatch(v.getDispatch())
	elif vt == jaut.Variant.VT_DISPATCH:
		return JautDispatch(v.getDispatch())
	elif vt == jaut.Variant.VT_UNKNOWN | jaut.Variant.VT_BYREF:
		return JautDispatch(v.getDispatch())
	elif vt == 0:
		return None
	elif vt == jaut.Variant.VT_I4:
		return v.getInt()
	# print 'yyy' + repr(v) + repr(type(v)) + '...' + str(vt)
	return v

def _arg_map(arg):
	if isinstance(arg, JautDispatch):
		return arg.dispatch
	return arg

class JautDispatch(object):
	@staticmethod
	def CoCreateInstance(progid, clsctx):
		dispatch = jaut.Dispatch()
		dispatch.attachNewInstance(progid, clsctx)
		return JautDispatch(dispatch)

	def __init__(self, dispatch, **kwargs):
		super(JautDispatch, self).__init__(self, **kwargs)
		super(JautDispatch, self).__setattr__('dispatch', dispatch)
	
	def __nonzero__(self):
		return self.dispatch.pDispatch != 0
		
	def __iter__(self):
		class JautIter(object):
			def __iter__(self):
				return self
			def next():
				pass
		# FIXME: call this by DISPID_NEWENUM==-4 instead
		# FIXME: need jaut.dll support for this
			# it = self.dispatch.get('_NewEnum')
		
		class GMEIter(object):
			def __iter__(self):
				return self
			def next(self):
				if self.i > self.coll.Count:
					raise StopIteration()
				ret = self.coll.Item(self.i)
				self.i = self.i + 1
				return ret
		iter = GMEIter()
		iter.i = 1
		iter.coll = self
		return iter

	def __setattr__(self, name, value):
		self.dispatch.put(name, _arg_map(value))
	
	def __getattr__(self, name):
		# fail fast
		id = self.dispatch.getIDOfName(name)
		try:
			ret = self.dispatch.get(name)
			#if isinstance(ret, jaut.Dispatch):
			#	return JautDispatch(ret)
			return _translate_java(ret)
		except org.isis.jaut.ComException:
			pass
		if _debug:
			_dbgout.write('__getattr__: name ' + name + "\n")
		def invoke(*args):
			import time
			args = [_arg_map(arg) for arg in args]
			if name == 'Status': # workaround error in typelib: propget without retval
				args = (jaut.Variant(jaut.Variant.create(jaut.Variant.VT_I4)),)
				self.dispatch.invoke(name, jaut.Dispatch.DISPATCH_PROPERTYGET, args, None)
				return args[0].getInt()
			if name in _getterkludge:
				return _translate_java(self.dispatch.invoke(name, jaut.Dispatch.DISPATCH_PROPERTYGET, args, None))
			if name in _argkludge:
				len_args = len(args)
				args = list(args)
				def kludgearg(t):
					if t == jaut.Variant.VT_BSTR|jaut.Variant.VT_ARRAY:
						return jaut.Variant()
					return None
				args.extend([jaut.Variant(jaut.Variant.create(t)) for t in _argkludge[name]])
				self.dispatch.callSub(name, args)
				ret = map(_translate_variant, args[len_args:])
				if len(ret) == 1:
					return ret[0]
				return ret
			return _translate_java(self.dispatch.call(name, args))
			
		return invoke


def DispatchEx(progid, clsctx=CLSCTX_LOCAL_SERVER):
    return JautDispatch.CoCreateInstance(progid, clsctx)
