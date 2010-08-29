simple = { --family house
    name            = "Simple", -- TODO localize
    expandable      = false, -- if true industry can expand or reduced
    required        = { passengers, mail }, -- refs or strings here?
    produced        = { passengers, mail }, -- 1, 2 
    callbacks       = {
         tick           = { },
         day            = { },
         month          = { },
    },
    
    -- internal registers, persistent
    _overcrowd      = 0, -- outdoors overcrowd, maybe calculated by bunches for quartals
    _wealth         = 0.5, -- ability to accept guests
    _guests         = 0, -- previously comes 
};

    --[[ 
        here should be available state of this subj, inputs and outputs
        
        for example if we have some inputs and coefficients for outputs production - 
        take some from input and put some to output
    ]]
    

simple.callbacks.day.economy = function( input, output )
    
    output[ 1 ] = 3; --minimum
    output[ 2 ] = 0.1; --1 mail per 10 days
    --[[ if regulary in this house visits by guests
        passangers number grows ]]
        
    --old guests
    if ( this._guests and math.rand < 0.2  ) then --here dynamic with _overcrowd fluence
        output[ 1 ] = output[ 1 ] + this._guests;
        this._guests = 0;
    end
    -- 
    if ( input[ 1 ] ) then --guests
        this._guests = this._guests + input[ 1 ];
        input[ 1 ] = 0; -- manufactured
        
    end
    -- mail
    if input[ 2 ] and math.rand < 0.3 then 
            output[ 2 ] =  output[ 2 ] + 1
    end
        
end
            
    