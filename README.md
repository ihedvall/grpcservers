# Google RPC Servers

## Summary

The repository implements a number of workflow tasks that uses the Google RPC interface. The raeson is that 
the gRPC library is difficult to link with libraries that use the OpenSSL encryption. Google is using another 
encryption library that doesn't link with the OpenSSL. It is possible to solve the problem but the linking 
of special build gRPC libraries is problematic.

The library is under development.
