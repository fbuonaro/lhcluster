#!/usr/bin/python3.6

from pkg_resources import resource_filename

ACTOR_FILTER_STR = 'and %Actor% contains "{}"'

SUPPORTED_SETTINGS = \
{
    'broker': True,
    'clientproxy': True,
    'notifypublisher': True,
    'notifysubscriber': True,
    'worker': True
}

class LoggingSettingsBuilder:
    def __init__( self ):
        self.settingsSegments = []
        self.cachedTemplates = {}

    def getSettingTemplate( self, templateName ):
        """
        Return the setting template with the specified name by either grabbing it
        from the cache or loading it from the file and caching it
        """
        settingsTemplate = self.cachedTemplates.get( templateName, None )
        if settingsTemplate is None:
            settingsTemplate = loadTemplateToCache( templateName )
        return settingsTemplate

    def loadTemplateToCache( self, templateName ):
        """
        Load the logging settings template with the specified name
        from the package logging config templates directory into the cachedTemplates
        """
        relativePackagePath = 'logging_config_templates/{}.ini'.format( templateName )
        realPackagePath = resource_filename( __name__, relativePackagePath )
        with open( realPackagePath, 'r' ) as packageFile:
            settingsTemplate = packageFile.read()
            self.cachedTemplates[ templateName ] = settingsTemplate
            return settingsTemplate

    def AddSettingsSegment( self, _settingsType, name, actor=None, directory="./" ):
        """
        Add logging settings
        Arguments:
            settingsType (str) - type of settings, could be broker, clientproxy, notifypublisher,
                                 notifysubscriber, or worker
            name (str) - alphanumeric string used to used to suffix the filename
        Keyword Arguments:
            actor (str) - a specific actor indicated by that actor's controller endpoint path
                          default to None (all actors of this type)
        Note that settings which do not include an actor will pick up all logs originating
        from any actor of this type
        """
        settingsType = _settingsType.lower()

        if( settingsType not in SUPPORTED_SETTINGS ):
            raise ValueError( "settings type '{}' is not supported".format( settingsType ) )

        settingsTemplate = self.getSettingTemplate( settingsType )
        templateParams = { 'name' : name, 'directory': directory }
        if actor is not None:
            templateParams[ 'actor_filter' ] = ACTOR_FILTER_STR.format( actor )
        self.settingsSegments.append( settingsTemplate.format( **templateParams ) )

    def BuildSettings( self ):
        """
        Creates a settings string that can be used to initialize logging process wide
        from the settings segments added
        """
        return "\n".join( self.settingsSegments )
