import unittest
import win32com.client
import win32ui
import pythoncom
import os
import gmeutils  # utility functions for GME testing

#####################################################
# MyFirstGMETestCase testing creation of objects
#####################################################
class MyFirstGMETestCase( unittest.TestCase ):
        """MyFirstGMETestCase tests creation of objects
        """
        
        """if you override __init__ method, call the base class's __init__ method:
        def __init__(self, methodName='runTest'):
                unittest.TestCase.__init__(self, methodName)
        """

        def setUp(self):
            """hook method, will be called before every test method
            put here the code you think is setup-like
            """
            
        def tearDown(self ):
            """hook method, will be called after each test method
            put here the common 'destructing' code if needed
            (however destructing explicitly is needed only for 
            resources like files, databases)
            """

        def runTest():
            """you may list here a sequence of test method invokations
            """
            testA()
            testB()

        def testA( self ):
            """DESCRIPTION: testing 
             CreateProject, CreateObject and finally comparing the xme output to an expected value
            the initial value of mydict
            """
            
            md = "_testA_sf_model1.mga"
            mganame = os.path.join( os.path.abspath(os.path.curdir), md)
            project = win32com.client.Dispatch("Mga.MgaProject")
            
            try:
                project.Create( "MGA=" + mganame, "SF")
            except:
                self.fail("Project Create failed")
                return
            
            terr = project.CreateTerritory( None, None, None)
            
            # 1st transaction: change project's and rootfolder's name
            trans = project.BeginTransaction( terr)

            project.Name = 'testA1_sf'

            rf = project.RootFolder
            rf.Name = 'testA1_sf'

            # commit transaction
            project.CommitTransaction()
            
            # 2nd transaction: create children
            trans = project.BeginTransaction( terr)

            metaproject = project.RootMeta
            metarootfolder = metaproject.RootFolder
            
            #metarootfolder.BeginTransaction()
            metafol = metarootfolder.LegalChildFolderByName('Folder')
            metafco1 = metarootfolder.LegalRootObjectByName('Compound')
            #metafco2 = metarootfolder.LegalRootObjectByName('InputSignal')
            meta_inp_sig = metarootfolder.DefinedFCOByName('InputSignal', 1)
            meta_out_sig = metarootfolder.DefinedFCOByName('OutputSignal', 1)
            #metarootfolder.CommitTransaction()

            newfolder = rf.CreateFolder( metafol)
            newfolder.Name = 'Folder1'
            
            newcomp1 = rf.CreateRootObject( metafco1)
            newcomp1.Name = 'Comp1'
            
            newcomp2 = newfolder.CreateRootObject( metafco1)
            newcomp2.Name = 'CompInFolder1'
            
            metarole_inp_sig = metafco1.RoleByName('InputSignals')
            metarole_out_sig = metafco1.RoleByName('OutputSignals')
            
            #newsig1 = newcomp2.CreateChildObject( metarole_inp_sig)
            #newsig1.Name = 'InSignal1'
            newcomp2.CreateChildObject( metarole_inp_sig).Name = 'InSignal1'

            #newsig2 = newcomp2.CreateChildObject( metarole_out_sig)
            #newsig2.Name = 'OutSignal1'
            newcomp2.CreateChildObject( metarole_out_sig).Name = 'OutSignal1'
            
            project.CommitTransaction()
            
            project.Save()
            project.Close(0)
            
            try:
                 xmename = gmeutils.mga2xme( mganame )
            except:
                raise 'Could not export file'

            # compare the just created file to a pre-created one: 'compareTo_testA_sf_model1.xme'
            ndiffs = gmeutils.xmeFileComp( xmename, 'compareTo_testA_sf_model1.xme')
            
            self.failIf( ndiffs == -1, 'Could not open some of the files')
            self.failIf( ndiffs > 0, str(ndiffs) + ' differencies between the two files ' + xmename + ' and ' + 'testA1_sf_model_to_compare_to.xme')
               
            pass

        def testB( self ):
            """DESCRIPTION: modifying Names inside the model created by testA
            """
            
            md = "_testA_sf_model1.mga"
            mganame = os.path.join( os.path.abspath(os.path.curdir), md)
            project = win32com.client.Dispatch("Mga.MgaProject")

            try:
            	project.Open( "MGA=" + mganame )
            except:
                self.fail('Could not open: ' + mganame)
                
            # 1st transaction
            trans = project.BeginTransaction(project.CreateTerritory( None, None, None))
            
            insig = gmeutils.findInProj( project, 'InSignal1')
            
            assert insig.Name == 'InSignal1'
            insig.Name = 'InputSign1'
            assert insig.Name == 'InputSign1'

            # commit transaction
            project.CommitTransaction()
            
            project.Save()
            project.Close(0)
           
            pass

#####################################################
# 1st option to produce testsuites
#####################################################
def suites():
    """A method named 'suites' is needed if you'd like to execute the tests with GPyunit.py
    Produces a test suite, gathering all methods in the class specified, matching the pattern specified:
    """
    suite = unittest.makeSuite(MyFirstGMETestCase,'test')
    return suite

#####################################################
# 2nd option to produce testsuites
#####################################################
def suites2():
    """Produces a test suite, adding methods one by one:
    """
    suite = unittest.TestSuite()
    suite.addTest( MyFirstGMETestCase( "testA"))
    suite.addTest( MyFirstGMETestCase( "testB"))
    return suite

########
# main
########
if __name__ == "__main__":
    """If this file is run standalone, execute the suite given back by the suites() function above
    """

    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TEST BEGINS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    
    ######## 1st option
    all = suites()
    runner = unittest.TextTestRunner()
    runner.run( all )

    ######## 2nd option
    #unittest.main(defaultTest='TestCaseX')
    
    ######## 3rd option
    #unittest.main()