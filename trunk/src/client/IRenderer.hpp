/* 
 * File:   i_renderer.hpp
 * Author: vaulter
 *
 * Created on 19 Август 2010 г., 23:00
 */

#ifndef I_RENDERER_HPP
#define	I_RENDERER_HPP

/**
 * Interface to main window of application
 */
namespace view {

    struct IRendererOptions {
        
    };

    class IRenderer {
        
    public:
        
        /** 
         * Initilising with params
         */
        virtual void Init( const IRendererOptions &aOptions ) throw() = 0 ;

        /**
         * Shows window. Here renderer can restore some data.
         */
        virtual void Show() = 0;
        
        /**
         * Closes window
         */
        virtual void Close() = 0;

        virtual void OnResize() = 0;


    };

}
#endif	/* I_RENDERER_HPP */

