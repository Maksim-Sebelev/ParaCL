module;

#include <vector>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <any>
#include <typeindex>
#include <functional>

export module node_type_erasure;

namespace ParaCL::ast::node
{

export
namespace visit_overload_set
{
template <typename NodeT, typename ReturnT, typename... Args>
ReturnT visit(NodeT const &node, Args... args)
{
    static_assert(false, "using not specialized visit function. "
                         "this static_assert help you to not think about terribale linker errors :)");
}
} /* visit_overload_set */

//--------------------------------------------------------------------------------------------------------------------------------------

// export
// class BasicNode
// {
// private:
//     struct IBaseNode
//     {
//         virtual ~IBaseNode() = default;
//         virtual std::unique_ptr<IBaseNode> clone_() const = 0;
//         virtual void write_(std::ofstream& ofs, size_t enclosure) const = 0;
//     };

//     template <typename ReturnT, typename... Args>
//     struct IVisitable : public IBaseNode
//     {
//         virtual ReturnT visit_(Args... args) const = 0;
//     };

//     template <typename NodeT, typename ReturnT, typename... Args>
//     struct Node final : public IVisitable<ReturnT, Args...>
//     {
//         NodeT data_;

//         Node(NodeT node) : data_(std::move(node))
//         {}

//         // Node(NodeT&& node) : data_(node)
//         // {}

//         std::unique_ptr<IBaseNode> clone_() const override
//         { return std::make_unique<Node<NodeT, ReturnT, Args...>>(*this); }

//         ReturnT visit_(Args... args) const override
//         { return visit_overload_set::template visit<NodeT, ReturnT, Args...>(data_, args...); }

//         void write_(std::ofstream& ofs, size_t enclosure) const override
//         { return visit_overload_set::template write<NodeT>(data_, ofs, enclosure); }

//         // template <typename OtherNodeT, typename OtherReturnT, typename... OtherArgs>
//         // friend struct Node;

//         template <typename OtherNodeT, typename OtherReturnT, typename... OtherArgs>
//         operator Node<OtherNodeT, OtherReturnT, OtherArgs...>()
//         { return Node<OtherNodeT, OtherReturnT, OtherArgs...>{data_}; }
//     };

//     std::unique_ptr<IBaseNode> self_ = nullptr;

//     // std::vector<std::unique_ptr<IBaseNode>> selfs_;
//     // std::vector<std::any> functions_;

// private:
//     BasicNode(std::unique_ptr<IBaseNode> node) : self_(std::move(node))
//     {}

// public:
//     template <typename ReturnT, typename... Args>
//     friend ReturnT visit(BasicNode const &node, Args... args)
//     {
//         using expected_node_type = IVisitable<ReturnT, Args...>;
//         auto&& self_ptr = dynamic_cast<const expected_node_type*>(node.self_.get());
        
//         // for (auto&& self: node.selfs_)
//         // {
//             // auto&& self_ptr = dynamic_cast<const expected_node_type*>(node.self_.get());
//             // if ()
//         // }

//         if (self_ptr) return self_ptr->visit_(args...);
//         throw std::runtime_error("Using fuction, not spezified for this node.");
//     }

//     friend void write(BasicNode const &node, std::ofstream& ofs, size_t enclosure)
//     { return node.self_->write_(ofs, enclosure); }

//     BasicNode() = default;

//     /* explicit - because for all node-type we need separate constructor */
//     // template <typename NodeT, typename ReturnT, typename... Args>
//     // explicit
//     // BasicNode(NodeT node) :
//         // self_(std::make_unique<Node<NodeT, ReturnT, Args...>>(std::move(node)))
//     // {}

//     template <typename NodeT, typename ReturnT, typename... Args>
//     static
//     BasicNode create(NodeT node)
//     { return BasicNode{std::make_unique<Node<NodeT, ReturnT, Args...>>(std::move(node))}; }

//     BasicNode(BasicNode const &other) :
//         self_(other.self_ ? other.self_->clone_() : nullptr)
//     {}

//     BasicNode &operator=(BasicNode const &other)
//     {
//         other.self_->clone_().swap(self_);
//         return *this;
//     }

//     BasicNode(BasicNode &&) = default;
//     BasicNode &operator=(BasicNode &&) = default;

// };



// export
// class BasicNode
// {
// private:
//     struct IBaseNode
//     {
//         virtual ~IBaseNode() = default;
//         virtual std::unique_ptr<IBaseNode> clone_() const = 0;
//         virtual void write_(std::ofstream& ofs, size_t enclosure) const = 0;
        
//         virtual std::any invoke(const std::type_info& sig, std::any* args, size_t num_args) const = 0;
//         virtual bool supports_signature(const std::type_info& sig) const = 0;
//     };

//     // Метафункция для проверки, является ли тип сигнатурой функции
//     template<typename T>
//     struct is_function_signature : std::false_type {};
    
//     template<typename Ret, typename... Args>
//     struct is_function_signature<Ret(Args...)> : std::true_type {};

//     // Вместо хранения сигнатур в tuple, будем использовать type_info
//     template<typename NodeT, typename... Signatures>
//     class NodeImpl final : public IBaseNode {
//     private:
//         NodeT data_;
        
//         // Убеждаемся, что все Signatures действительно сигнатуры функций
//         static_assert((is_function_signature<Signatures>::value && ...), 
//                       "All template parameters must be function signatures");

//         // Вспомогательная структура для вызова функции с определенной сигнатурой
//         template<typename Signature>
//         struct Invoker;
        
//         template<typename ReturnT, typename... Args>
//         struct Invoker<ReturnT(Args...)> {
//             static std::any call(const NodeT& data, std::any* args) {
//                 try {
//                     return call_impl(data, args, std::index_sequence_for<Args...>{});
//                 } catch (const std::bad_any_cast& e) {
//                     throw std::runtime_error(
//                         std::string("Type mismatch in arguments for signature ") + 
//                         typeid(ReturnT(Args...)).name() + ": " + e.what()
//                     );
//                 }
//             }
            
//         private:
//             template<size_t... Is>
//             static std::any call_impl(const NodeT& data, std::any* args, std::index_sequence<Is...>) {
//                 if constexpr (std::is_void_v<ReturnT>) {
//                     visit_overload_set::template visit<NodeT, ReturnT, Args...>(
//                         data, 
//                         std::any_cast<Args>(args[Is])...
//                     );
//                     return std::any{};
//                 } else {
//                     auto result = visit_overload_set::template visit<NodeT, ReturnT, Args...>(
//                         data, 
//                         std::any_cast<Args>(args[Is])...
//                     );
//                     return std::any(std::move(result));
//                 }
//             }
//         };

//         // Вспомогательная функция для проверки поддержки сигнатуры
//         template<size_t... Is>
//         bool supports_signature_impl(const std::type_info& sig, std::index_sequence<Is...>) const {
//             // Проверяем каждую сигнатуру из списка
//             bool supported = false;
//             // Используем fold expression с запятой
//             ((typeid(Signatures) == sig ? (supported = true) : false), ...);
//             return supported;
//         }

//         template<size_t... Is>
//         std::any invoke_impl(const std::type_info& sig, std::any* args, size_t num_args, 
//                              std::index_sequence<Is...>) const {
//             std::any result;
//             bool found = false;
            
//             // Пробуем каждую сигнатуру
//             ((typeid(Signatures) == sig ? 
//               (found = true, result = invoke_one<Signatures>(args, num_args)) : 
//               false), ...);
            
//             if (found) {
//                 return result;
//             }
            
//             // Формируем сообщение об ошибке со списком поддерживаемых сигнатур
//             std::string supported;
//             ((supported += typeid(Signatures).name(), supported += " "), ...);
//             throw std::runtime_error(
//                 std::string("Signature ") + sig.name() + 
//                 " not supported by this node. Supported: " + supported
//             );
//         }

//         template<typename Signature>
//         std::any invoke_one(std::any* args, size_t num_args) const {
//             return Invoker<Signature>::call(data_, args);
//         }

//     public:
//         explicit NodeImpl(NodeT node) : data_(std::move(node)) {}

//         std::unique_ptr<IBaseNode> clone_() const override {
//             // Явно преобразуем unique_ptr<NodeImpl> в unique_ptr<IBaseNode>
//             return std::unique_ptr<IBaseNode>(
//                 std::make_unique<NodeImpl>(*this).release()
//             );
//         }

//         void write_(std::ofstream& ofs, size_t enclosure) const override {
//             visit_overload_set::template write<NodeT>(data_, ofs, enclosure);
//         }

//         std::any invoke(const std::type_info& sig, std::any* args, size_t num_args) const override {
//             return invoke_impl(sig, args, num_args, std::index_sequence_for<Signatures...>{});
//         }

//         bool supports_signature(const std::type_info& sig) const override {
//             return supports_signature_impl(sig, std::index_sequence_for<Signatures...>{});
//         }
//     };

//     std::unique_ptr<IBaseNode> self_;

//     explicit BasicNode(std::unique_ptr<IBaseNode> self) : self_(std::move(self)) {}

// public:
//     BasicNode() = default;

//     // ШАБЛОННЫЙ МЕТОД CREATE - ЗДЕСЬ УКАЗЫВАЮТСЯ ВСЕ ПОДДЕРЖИВАЕМЫЕ СИГНАТУРЫ
//     template<typename NodeT, typename... Signatures>
//     static BasicNode create(NodeT node) {
//         static_assert(sizeof...(Signatures) > 0, 
//                       "At least one signature must be specified");
//         // Явно преобразуем unique_ptr в базовый тип
//         auto impl = std::make_unique<NodeImpl<NodeT, Signatures...>>(std::move(node));
//         return BasicNode(std::unique_ptr<IBaseNode>(impl.release()));
//     }

//     // Внешняя функция visit
//     template<typename ReturnT, typename... Args>
//     friend ReturnT visit(BasicNode const& node, Args... args)
//     {
//         if (!node.self_) {
//             throw std::runtime_error("Cannot visit empty node");
//         }
        
//         // Проверяем поддержку сигнатуры перед вызовом
//         if (!node.self_->supports_signature(typeid(ReturnT(Args...)))) {
//             throw std::runtime_error(
//                 std::string("Signature ") + typeid(ReturnT(Args...)).name() + 
//                 " is not supported by this node"
//             );
//         }

//         std::any arg_array[] = {std::any(std::forward<Args>(args))...};
//         auto result = node.self_->invoke(
//             typeid(ReturnT(Args...)), 
//             arg_array, 
//             sizeof...(Args)
//         );

//         if constexpr (!std::is_void_v<ReturnT>) {
//             try {
//                 return std::any_cast<ReturnT>(result);
//             } catch (const std::bad_any_cast& e) {
//                 throw std::runtime_error(
//                     std::string("Return type mismatch: expected ") + 
//                     typeid(ReturnT).name() + 
//                     " but got " + 
//                     result.type().name()
//                 );
//             }
//         }
//     }

//     friend void write(BasicNode const& node, std::ofstream& ofs, size_t enclosure) {
//         if (node.self_) node.self_->write_(ofs, enclosure);
//     }

//     // Проверка поддержки конкретной сигнатуры
//     template<typename ReturnT, typename... Args>
//     bool supports() const {
//         if (!self_) return false;
//         return self_->supports_signature(typeid(ReturnT(Args...)));
//     }

//     // Копирование и перемещение
//     BasicNode(BasicNode const& other) 
//         : self_(other.self_ ? other.self_->clone_() : nullptr) {}
    
//     BasicNode& operator=(BasicNode const& other) {
//         if (this != &other) {
//             self_ = other.self_ ? other.self_->clone_() : nullptr;
//         }
//         return *this;
//     }
    
//     BasicNode(BasicNode&&) = default;
//     BasicNode& operator=(BasicNode&&) = default;
// };


export
class BasicNode
{
private:
    struct IBaseNode
    {
        virtual ~IBaseNode() = default;
        virtual std::unique_ptr<IBaseNode> clone_() const = 0;
        // virtual void write_(std::ofstream& ofs, size_t enclosure) const = 0;
        
        virtual std::any invoke(const std::type_info& sig, std::any* args, size_t num_args) const = 0;
        virtual bool supports_signature(const std::type_info& sig) const = 0;
    };

    // Метафункция для проверки, является ли тип сигнатурой функции
    template<typename T>
    struct is_function_signature : std::false_type {};
    
    template<typename Ret, typename... Args>
    struct is_function_signature<Ret(Args...)> : std::true_type {};

    template<typename NodeT, typename... Signatures>
    class NodeImpl final : public IBaseNode {
    private:
        NodeT data_;
        
        static_assert((is_function_signature<Signatures>::value && ...), 
                      "All template parameters must be function signatures");

        // Вспомогательная структура для работы с ссылками в std::any
        template<typename T>
        struct ArgWrapper {
            using Type = T;
        };
        
        // Для ссылок используем reference_wrapper
        template<typename T>
        struct ArgWrapper<T&> {
            using Type = std::reference_wrapper<T>;
        };

        template<typename Signature>
        struct Invoker;
        
        template<typename ReturnT, typename... Args>
        struct Invoker<ReturnT(Args...)> {
            static std::any call(const NodeT& data, std::any* args) {
                try {
                    return call_impl(data, args, std::index_sequence_for<Args...>{});
                } catch (const std::bad_any_cast& e) {
                    throw std::runtime_error(
                        std::string("Type mismatch in arguments for signature ") + 
                        typeid(ReturnT(Args...)).name() + ": " + e.what()
                    );
                }
            }
            
        private:
            // Вспомогательная функция для распаковки аргумента
            template<typename T>
            static T unwrap_arg(const std::any& arg) {
                if constexpr (std::is_reference_v<T>) {
                    // Для ссылок используем reference_wrapper
                    using WrappedType = std::reference_wrapper<std::remove_reference_t<T>>;
                    auto& wrapper = std::any_cast<WrappedType&>(const_cast<std::any&>(arg));
                    return wrapper.get();
                } else {
                    // Для значений просто копируем
                    return std::any_cast<T>(arg);
                }
            }
            
            template<size_t... Is>
            static std::any call_impl(const NodeT& data, std::any* args, std::index_sequence<Is...>) {
                if constexpr (std::is_void_v<ReturnT>) {
                    visit_overload_set::template visit<NodeT, ReturnT, Args...>(
                        data, 
                        unwrap_arg<Args>(args[Is])...
                    );
                    return std::any{};
                } else {
                    auto result = visit_overload_set::template visit<NodeT, ReturnT, Args...>(
                        data, 
                        unwrap_arg<Args>(args[Is])...
                    );
                    return std::any(std::move(result));
                }
            }
        };

        template<size_t... Is>
        bool supports_signature_impl(const std::type_info& sig, std::index_sequence<Is...>) const {
            bool supported = false;
            ((typeid(Signatures) == sig ? (supported = true) : false), ...);
            return supported;
        }

        template<size_t... Is>
        std::any invoke_impl(const std::type_info& sig, std::any* args, size_t num_args, 
                             std::index_sequence<Is...>) const {
            std::any result;
            bool found = false;
            
            ((typeid(Signatures) == sig ? 
              (found = true, result = invoke_one<Signatures>(args, num_args)) : 
              false), ...);
            
            if (found) {
                return result;
            }
            
            std::string supported;
            ((supported += typeid(Signatures).name(), supported += " "), ...);
            throw std::runtime_error(
                std::string("Signature ") + sig.name() + 
                " not supported by this node. Supported: " + supported
            );
        }

        template<typename Signature>
        std::any invoke_one(std::any* args, size_t num_args) const {
            return Invoker<Signature>::call(data_, args);
        }

    public:
        explicit NodeImpl(NodeT node) : data_(std::move(node)) {}

        std::unique_ptr<IBaseNode> clone_() const override {
            return std::unique_ptr<IBaseNode>(
                std::make_unique<NodeImpl>(*this).release()
            );
        }

        // void write_(std::ofstream& ofs, size_t enclosure) const override {
            // visit_overload_set::template write<NodeT>(data_, ofs, enclosure);
        // }

        std::any invoke(const std::type_info& sig, std::any* args, size_t num_args) const override {
            return invoke_impl(sig, args, num_args, std::index_sequence_for<Signatures...>{});
        }

        bool supports_signature(const std::type_info& sig) const override {
            return supports_signature_impl(sig, std::index_sequence_for<Signatures...>{});
        }
    };

    std::unique_ptr<IBaseNode> self_;

    explicit BasicNode(std::unique_ptr<IBaseNode> self) : self_(std::move(self)) {}

    // Вспомогательная функция для упаковки аргументов
    template<typename T>
    static std::any pack_arg(T&& arg) {
        if constexpr (std::is_lvalue_reference_v<T>) {
            // Для lvalue ссылок используем reference_wrapper
            return std::ref(arg);
        } else {
            // Для значений и rvalue ссылок просто копируем/перемещаем
            return std::any(std::forward<T>(arg));
        }
    }

public:
    BasicNode() = default;

    template<typename NodeT, typename... Signatures>
    static BasicNode create(NodeT node) {
        static_assert(sizeof...(Signatures) > 0, 
                      "At least one signature must be specified");
        auto impl = std::make_unique<NodeImpl<NodeT, Signatures...>>(std::move(node));
        return BasicNode(std::unique_ptr<IBaseNode>(impl.release()));
    }

    // Внешняя функция visit
    template<typename ReturnT, typename... Args>
    friend ReturnT visit(BasicNode const& node, Args... args)
    {
        if (!node.self_) {
            throw std::runtime_error("Cannot visit empty node");
        }
        
        using SigType = ReturnT(Args...);
        
        if (!node.self_->supports_signature(typeid(SigType))) {
            throw std::runtime_error(
                std::string("Signature ") + typeid(SigType).name() + 
                " is not supported by this node"
            );
        }
        
        // Упаковываем аргументы, правильно обрабатывая ссылки
        std::any arg_array[] = {pack_arg(std::forward<Args>(args))...};
        auto result = node.self_->invoke(
            typeid(SigType), 
            arg_array, 
            sizeof...(Args)
        );

        if constexpr (!std::is_void_v<ReturnT>) {
            try {
                return std::any_cast<ReturnT>(result);
            } catch (const std::bad_any_cast& e) {
                throw std::runtime_error(
                    std::string("Return type mismatch: expected ") + 
                    typeid(ReturnT).name() + 
                    " but got " + 
                    result.type().name()
                );
            }
        }
    }

    // friend void write(BasicNode const& node, std::ofstream& ofs, size_t enclosure) {
        // if (node.self_) node.self_->write_(ofs, enclosure);
    // }

    template<typename ReturnT, typename... Args>
    bool supports() const {
        if (!self_) return false;
        return self_->supports_signature(typeid(ReturnT(Args...)));
    }

    // Копирование и перемещение
    BasicNode(BasicNode const& other) 
        : self_(other.self_ ? other.self_->clone_() : nullptr) {}
    
    BasicNode& operator=(BasicNode const& other) {
        if (this != &other) {
            self_ = other.self_ ? other.self_->clone_() : nullptr;
        }
        return *this;
    }
    
    BasicNode(BasicNode&&) = default;
    BasicNode& operator=(BasicNode&&) = default;
};

//--------------------------------------------------------------------------------------------------------------------------------------
} /* namespace ParaCL::ast::node */
//--------------------------------------------------------------------------------------------------------------------------------------
