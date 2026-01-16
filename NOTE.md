## Supported Features

The configuration file supports the following features:

- Definition of one or more listening ports per server block
- Definition of default error pages at the server level
- Definition of a maximum allowed size for client request bodies
- Definition of location blocks associated with a URL path
- Per-location list of accepted HTTP methods
- HTTP redirection rules for a location
- Definition of the directory where requested resources are located
- Enabling or disabling directory listing
- Definition of a default file to serve when a directory is requested
- Authorization of file uploads and definition of the upload storage path

## Validation Rules
 
Unknown directives, invalid values, or syntax errors result in a configuration  
error and prevent the server from starting.
