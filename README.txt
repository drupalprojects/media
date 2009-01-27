// $Id$

-------------------------------------------------------
 Media Module
-------------------------------------------------------

Initial development January 2009 by Arthur Foelsche (arthurf).

A GUI for file management for upload forms.

The Media module provides a drop-in replacement for Drupal's Upload, FileField,
Embedded Media Field, and other similar forms. It offers an API and hooks
available for other modules to implement, allowing for customized file lists,
tabs, drawers, and forms to the new Upload form.

INSTALLATION
-------------------------------------------------------
 1. Place module in your modules directory.
 2. Go to admin > build > modules and enable media module.

CONFIGURATION
-------------------------------------------------------
 1. Go to admin > content > media
 2. Configure settings per content type. Each field in a node can have options
    for what resources and formatters are available to it.


DEVELOPERS
-------------------------------------------------------
We are documenting the API as we go. Please see: http://drupal.org/node/356803

@TODO
-------------------------------------------------------
 * Currently requires the Tabs module. May want to remove that dependency.
 * Remove media mover code.
