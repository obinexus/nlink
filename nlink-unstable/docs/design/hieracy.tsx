import React from 'react';

const ArchitectureDiagram = () => {
  const moduleStyle = {
    border: '2px solid #4a6fa5',
    borderRadius: '6px',
    padding: '10px',
    margin: '10px',
    backgroundColor: '#e6f0ff',
    boxShadow: '0 4px 6px rgba(0,0,0,0.1)',
    maxWidth: '200px',
    minHeight: '80px',
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'center',
    alignItems: 'center',
    textAlign: 'center',
    fontSize: '14px'
  };
  
  const headerStyle = {
    fontWeight: 'bold',
    marginBottom: '5px',
    fontSize: '16px'
  };
  
  const containerStyle = {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'stretch',
    padding: '20px',
    border: '1px solid #ccc',
    borderRadius: '8px',
    backgroundColor: '#f8f9fa'
  };
  
  const rowStyle = {
    display: 'flex',
    justifyContent: 'center',
    marginBottom: '15px'
  };

  const arrowContainerStyle = {
    display: 'flex',
    justifyContent: 'center',
    height: '30px',
    position: 'relative'
  };

  const verticalArrowStyle = {
    width: '2px',
    height: '100%',
    backgroundColor: '#555',
    position: 'absolute'
  };

  const arrowDownStyle = {
    width: '0',
    height: '0',
    borderLeft: '8px solid transparent',
    borderRight: '8px solid transparent',
    borderTop: '8px solid #555',
    position: 'absolute',
    bottom: '0'
  };

  const horizontalLineStyle = {
    height: '2px',
    backgroundColor: '#555',
    flexGrow: 1,
    margin: '0 10px',
    position: 'relative'
  };

  const arrowRightStyle = {
    width: '0',
    height: '0',
    borderTop: '8px solid transparent',
    borderBottom: '8px solid transparent',
    borderLeft: '8px solid #555',
    position: 'absolute',
    right: '0',
    top: '-7px'
  };

  return (
    <div style={containerStyle}>
      <h3 style={{textAlign: 'center', marginBottom: '20px'}}>NexusLink Architecture</h3>
      
      {/* Top layer: Common */}
      <div style={rowStyle}>
        <div style={moduleStyle}>
          <div style={headerStyle}>Common</div>
          <div>types.h, result.h</div>
          <div>Base definitions</div>
        </div>
      </div>
      
      <div style={arrowContainerStyle}>
        <div style={verticalArrowStyle}></div>
        <div style={arrowDownStyle}></div>
      </div>
      
      {/* Second layer: Core */}
      <div style={rowStyle}>
        <div style={moduleStyle}>
          <div style={headerStyle}>Core</div>
          <div>nexus_core.h</div>
          <div>Context management</div>
        </div>
      </div>
      
      <div style={arrowContainerStyle}>
        <div style={verticalArrowStyle}></div>
        <div style={arrowDownStyle}></div>
      </div>
      
      {/* Third layer: Symbols, JSON, Versioning */}
      <div style={rowStyle}>
        <div style={moduleStyle}>
          <div style={headerStyle}>Symbols</div>
          <div>registry.h, symbols.h</div>
          <div>Symbol management</div>
        </div>
        
        <div style={{width: '20px'}}></div>
        
        <div style={moduleStyle}>
          <div style={headerStyle}>JSON</div>
          <div>nexus_json.h</div>
          <div>Data serialization</div>
        </div>
        
        <div style={{width: '20px'}}></div>
        
        <div style={moduleStyle}>
          <div style={headerStyle}>Versioning</div>
          <div>semver.h</div>
          <div>Version constraints</div>
        </div>
      </div>
      
      <div style={arrowContainerStyle}>
        <div style={verticalArrowStyle}></div>
        <div style={arrowDownStyle}></div>
      </div>
      
      {/* Fourth layer: Metadata, Lazy Loading */}
      <div style={rowStyle}>
        <div style={moduleStyle}>
          <div style={headerStyle}>Metadata</div>
          <div>enhanced_metadata.h</div>
          <div>Component tracking</div>
        </div>
        
        <div style={{width: '20px'}}></div>
        
        <div style={moduleStyle}>
          <div style={headerStyle}>Lazy Loading</div>
          <div>lazy_versioned.h</div>
          <div>On-demand loading</div>
        </div>
      </div>
      
      <div style={arrowContainerStyle}>
        <div style={verticalArrowStyle}></div>
        <div style={arrowDownStyle}></div>
      </div>
      
      {/* Fifth layer: Minimizer */}
      <div style={rowStyle}>
        <div style={moduleStyle}>
          <div style={headerStyle}>Minimizer</div>
          <div>okpala_minimizer.h</div>
          <div>State optimization</div>
        </div>
      </div>
      
      <div style={arrowContainerStyle}>
        <div style={verticalArrowStyle}></div>
        <div style={arrowDownStyle}></div>
      </div>
      
      {/* Sixth layer: CLI */}
      <div style={rowStyle}>
        <div style={moduleStyle}>
          <div style={headerStyle}>CLI</div>
          <div>nlink_cli.h</div>
          <div>Command interface</div>
        </div>
      </div>
    </div>
  );
};

export default ArchitectureDiagram;