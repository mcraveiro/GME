# GME Python Component Framework
# (c) 2002-2006 ISIS, Vanderbilt University

InterpreterTemplate = """\
from GMEComComponent import GMEComComponent

class %(name)s(GMEComComponent):

	# Component specific parts for COM registration
	_comname_ = "%(name)s"
	_comp_version_ = "%(version)s"
	_reg_clsid_ = "%(clsid)s"
	_reg_iconfile_ = "%(iconpath)s"
	
	_reg_desc_ = _comname_
	_reg_progid_ = "MGA.PythonInterpreter.%(name)s"
	_reg_verprogid_ = "MGA.PythonInterpreter.%(name)s.%(version)s"
	_reg_class_spec_ = "%%s.%%s" %% (__module__, _comname_)

	# Component specific parts for GME registration
	_tooltip_ = "%(tooltip)s"
	_paradigm_ = "%(paradigm)s"

	def InvokeEx(self, project, currentobj, selectedobjs, param):
		super(self.__class__, self).InvokeEx(project)
		aborted = False
		try:
			self.project.BeginTransaction(self.project.CreateTerritory(None, None, None))
			currentobj = currentobj and self.mga.IMgaModel(currentobj)

			# component work goes here
			self.Logger("Python component: %%s" %% self._comname_)
		finally:
			if not aborted:
				self.project.CommitTransaction()
"""

AddonTemplate = """\
from GMEComComponent import GMEComComponent
import winerror
from win32com.server.util import wrap

class %(name)s(GMEComComponent):

	_typelib_guid_ = '{270B4F86-B17C-11D3-9AD1-00AA00B6FE26}'
	_typelib_version = 1,0
	_com_interfaces_ = ['IMgaEventSink']

	# Component specific parts for COM registration
	_comname_ = "%(name)s"
	_comp_version_ = "%(version)s"
	_reg_clsid_ = "%(clsid)s"
	
	_reg_desc_ = _comname_
	_reg_progid_ = "MGA.PythonAddon.%%s" %% _comname_
	_reg_verprogid_ = "MGA.PythonAddon.%%s.%%s" %% (_comname_, _comp_version_)
	_reg_class_spec_ = "%%s.%%s" %% (__module__, _comname_)

	# Component specific parts for GME registration
	_component_type_ = GMEComComponent.mga.constants.COMPONENTTYPE_ADDON
	_paradigm_ = "%(paradigm)s"

	def Initialize(self, project):
		self.Project = self.mga.IMgaProject(project)
		addon = self.Project.CreateAddOn(wrap(self))
		self.Addon = self.mga.IMgaAddOn(addon)
		# Set this mask to the object events for which the Addon requires notification
		# Constants are defined in Mga.idl
		self.Addon.EventMask = self.mga.constants.OBJEVENT_CREATED + \\
							   self.mga.constants.OBJEVENT_LOSTCHILD + \\
							   self.mga.constants.OBJEVENT_ATTR + \\
							   self.mga.constants.OBJEVENT_PROPERTIES + \\
							   self.mga.constants.OBJEVENT_CLOSEMODEL + \\
							   self.mga.constants.OBJEVENT_REGISTRY

	def GlobalEvent(self, event):
		# Example processing
		# Constants are defined in Mga.idl
		#try:
		#	if event == self.mga.constants.GLOBALEVENT_OPEN_PROJECT:
		#		pass
		#	elif event == self.mga.constants.GLOBALEVENT_CLOSE_PROJECT:
		#		pass
		#except Exception, e:
		#	pass
		return winerror.S_OK

	def ObjectEvent(self, obj, mask, v):
		# Example processing
		# Constants are defined in Mga.idl
		#try:
		#	obj = self.mga.IMgaFCO(obj)
		#	if mask & self.mga.constants.OBJEVENT_CREATED:
		#		pass
		#	if mask & self.mga.constants.OBJEVENT_LOSTCHILD:
		#		pass
		#	if mask & self.mga.constants.OBJEVENT_ATTR:
		#		pass
		#except Exception, e:
		#	pass
		return winerror.S_OK
"""
