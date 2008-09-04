/************************************************************

An API for simulating classical inheritence with JS functions.

Code taken from the englightening article on the topic
by Douglas Crockford:

  http://www.crockford.com/javascript/inheritance.html

************************************************************/

/************************************************************

Maps the func object as a member
function of this, with the given function name.

example:

MyClass.method( 'doSomething', function() { ... });

When the function is called, via myobj.doSomething(),
the 'this' reference is resolved as expected.
************************************************************/
Function.prototype.method = function (name, func) {
    this.prototype[name] = func;
    return this;
};

/************************************************************
inherits() must be called for types which inherit another,
as shown here:

// parent class:
function TypeA() { ... }

// subclasss:
function TypeB() { ... }

TypeB.inherits(TypeA);

TypeB is now a TypeA, such that (myB instanceof TypeA) will evaluate
to true.

************************************************************/
Function.method('inherits', function (parent) {
    var d = 0, p = (this.prototype = new parent());
    this.method('uber', function(name) {
        var f, r, t = d, v = parent.prototype;
        if (t) {
            while (t) {
                v = v.constructor.prototype;
                t -= 1;
            }
            f = v[name];
        } else {
            f = p[name];
            if (f == this[name]) {
                f = v[name];
            }
        }
        d += 1;
        r = f.apply(this, Array.prototype.slice.apply(arguments, [1]));
        d -= 1;
        return r;
    });
    return this;
});

/************************************************************
Loops through function call arguments. For each argument name, it
assigns parent.prototype[name] to this.prototype[name]. This effectively
copies selected pieces of parent's API to this class.
************************************************************/
Function.method('swiss', function (parent) {
    for (var i = 1; i < arguments.length; i += 1) {
        var name = arguments[i];
        this.prototype[name] = parent.prototype[name];
    }
    return this;
});



////////////////////////////////////////////////////////////////////////
// Example of how to use the Function API to create classes...
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
if( 0 /* enable/disable demo code */  ) {

function Parenizor(value) {
    this.setValue(value);
}

Parenizor.method('setValue', function (value) {
    this.value = value;
    return this;
});

Parenizor.method('getValue', function () {
    return this.value;
});

Parenizor.method('toString', function () {
    return '(' + this.getValue() + ')';
});

function ZParenizor(value) {
    this.setValue(value);
}

ZParenizor.inherits(Parenizor);

ZParenizor.method('toString', function () {
    if (this.getValue()) {
        return this.uber('toString');
    }
    return "-0-";
});


print( (new Parenizor(3)).toString() );
print( (new Parenizor(0)).toString() );

print( (new ZParenizor(3)).toString() );
print( (new ZParenizor(0)).toString() );

} // demonstration
////////////////////////////////////////////////////////////////////////
1;
