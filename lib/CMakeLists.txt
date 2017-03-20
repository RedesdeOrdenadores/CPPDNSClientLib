add_library (dnsclient ResourceRecord.cc DomainName.cc AResourceRecord.cc AAAAResourceRecord.cc NSResourceRecord.cc Message.cc)
target_compile_features(dnsclient PRIVATE cxx_auto_type cxx_constexpr cxx_range_for cxx_decltype_auto)

