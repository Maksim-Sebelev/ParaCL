module;

#include <memory>
#include <stdexcept>
#include <any>
#include <type_traits>

export module node_type_erasure;

namespace ParaCL::node
{

export
namespace visit_overload_set
{
template <typename NodeT, typename ReturnT, typename... Args>
ReturnT visit(NodeT const &node, Args... args)
{
    static_assert(false, "using not specialized visit function. "
                         "this static_assert help you to not see terribale linker errors :)");
}
} /* visit_overload_set */

//--------------------------------------------------------------------------------------------------------------------------------------

export
class BasicNode
{
private:
    struct IBaseNode
    {
        virtual ~IBaseNode() = default;
        virtual std::unique_ptr<IBaseNode> clone_() const = 0;

        virtual std::any invoke(const std::type_info& sig, std::any* args, size_t num_args) const = 0;
        virtual bool supports_signature(const std::type_info& sig) const = 0;
    };

    template<typename NodeT, typename... Signatures>
    requires (std::is_function_v<Signatures> && ...)
    class NodeImpl final : public IBaseNode
    {
    private:
        NodeT data_;

        template<typename Signature>
        struct Invoker;

        template<typename ReturnT, typename... Args>
        struct Invoker<ReturnT(Args...)> {
            static std::any call(const NodeT& data, std::any* args) try
            { return call_impl(data, args, std::index_sequence_for<Args...>{}); }
            catch (const std::bad_any_cast& e)
            {
                throw std::runtime_error(
                    std::string("Type mismatch in arguments for signature ") + 
                    typeid(ReturnT(Args...)).name() + ": " + e.what()
                );
            }

        private:
            template<typename T>
            static T unwrap_arg(const std::any& arg)
            {
                if constexpr (std::is_reference_v<T>)
                {
                    using WrappedType = std::reference_wrapper<std::remove_reference_t<T>>;
                    auto& wrapper = std::any_cast<WrappedType&>(const_cast<std::any&>(arg));
                    return wrapper.get();
                }
                else
                    return std::any_cast<T>(arg);
            }

            template<size_t... Is>
            static std::any call_impl(const NodeT& data, std::any* args, std::index_sequence<Is...>)
            {
                if constexpr (std::is_void_v<ReturnT>)
                {
                    visit_overload_set::template visit<NodeT, ReturnT, Args...>(
                        data, 
                        unwrap_arg<Args>(args[Is])...
                    );
                    return std::any{};
                }
                else
                {
                    auto result = visit_overload_set::template visit<NodeT, ReturnT, Args...>(
                        data, 
                        unwrap_arg<Args>(args[Is])...
                    );
                    return std::any(std::move(result));
                }
            }
        };

        template<size_t... Is>
        bool supports_signature_impl(const std::type_info& sig, std::index_sequence<Is...>) const
        {
            bool supported = false;
            ((typeid(Signatures) == sig ? (supported = true) : false), ...);
            return supported;
        }

        template<size_t... Is>
        std::any invoke_impl(const std::type_info& sig, std::any* args, size_t num_args, 
                             std::index_sequence<Is...>) const
        {
            std::any result;
            bool found = false;

            ((typeid(Signatures) == sig ? 
              (found = true, result = invoke_one<Signatures>(args, num_args)) : 
              false), ...);

            if (found) return result;

            std::string supported;
            ((supported += typeid(Signatures).name(), supported += " "), ...);
            throw std::runtime_error(
                std::string("Signature ") + sig.name() + 
                " not supported by this node. Supported: " + supported
            );
        }

        template<typename Signature>
        std::any invoke_one(std::any* args, size_t num_args) const
        { return Invoker<Signature>::call(data_, args); }

    public:
        explicit NodeImpl(NodeT node) : data_(std::move(node)) {}

        std::unique_ptr<IBaseNode> clone_() const override
        {
            return std::unique_ptr<IBaseNode>(
                std::make_unique<NodeImpl>(*this).release()
            );
        }

        std::any invoke(const std::type_info& sig, std::any* args, size_t num_args) const override
        { return invoke_impl(sig, args, num_args, std::index_sequence_for<Signatures...>{}); }

        bool supports_signature(const std::type_info& sig) const override
        { return supports_signature_impl(sig, std::index_sequence_for<Signatures...>{}); }
    };

    std::unique_ptr<IBaseNode> self_ = nullptr;

    explicit BasicNode(std::unique_ptr<IBaseNode> self) : self_(std::move(self)) {}

    template<typename T>
    static std::any pack_arg(T&& arg)
    {
        if constexpr (std::is_lvalue_reference_v<T>)
            return std::ref(arg);
        else
            return std::any(std::forward<T>(arg));
    }

public:
    BasicNode() = default;

    /* check that node support some visit functions */
    template<typename... Signatures>
    requires (std::is_function_v<Signatures> && ...)
    friend bool support(BasicNode const & node)
    {
        if (!node.self_) return false;
        return (node.self_->supports_signature(typeid(Signatures)) && ...);
    }

    template<typename NodeT, typename... Signatures>
    requires (std::is_function_v<Signatures> && ...)
    static BasicNode create(NodeT node)
    {
        static_assert(sizeof...(Signatures) > 0, 
                      "At least one signature must be specified");
        auto impl = std::make_unique<NodeImpl<NodeT, Signatures...>>(std::move(node));
        return BasicNode(std::unique_ptr<IBaseNode>(impl.release()));
    }

    template<typename ReturnT, typename... Args>
    friend ReturnT visit(BasicNode const& node, Args... args)
    {
        if (!node.self_) return;

        using SigType = ReturnT(Args...);

        if (!node.self_->supports_signature(typeid(SigType))) 
        {
            throw std::runtime_error(
                std::string("This node dont support this function: ") + typeid(SigType).name()
            );
        }

        std::any arg_array[] = {pack_arg(std::forward<Args>(args))...};
        auto result = node.self_->invoke(
            typeid(SigType), 
            arg_array, 
            sizeof...(Args)
        );

        if constexpr (!std::is_void_v<ReturnT>)
        {
            try
            {
                return std::any_cast<ReturnT>(result);
            }
            catch (const std::bad_any_cast& e)
            {
                throw std::runtime_error(
                    std::string("Return type mismatch: expected ") + 
                    typeid(ReturnT).name() + 
                    " but got " + 
                    result.type().name()
                );
            }
        }
    }

    BasicNode(BasicNode const& other) 
        : self_(other.self_ ? other.self_->clone_() : nullptr)
    {}

    BasicNode& operator=(BasicNode const& other)
    {
        if (this == &other) return *this;
        self_ = other.self_ ? other.self_->clone_() : nullptr;
        return *this;
    }

    BasicNode(BasicNode&&) = default;
    BasicNode& operator=(BasicNode&&) = default;
};

//--------------------------------------------------------------------------------------------------------------------------------------
} /* namespace ParaCL::node */
//--------------------------------------------------------------------------------------------------------------------------------------
